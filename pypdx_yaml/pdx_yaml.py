import copy
import ctypes
import os

# 将编译后的动态链接库放置于此
# 并将该处的动态链接库名字改成正确的文件名
__lib_path__ = os.path.dirname(__file__) + "/libpdx_yaml_py.so"
__lib__ = ctypes.cdll.LoadLibrary(__lib_path__)


def load_function_ptr(func_name: str, arg_types: list, ret_type):
    f_ptr = __lib__.__getattr__(func_name)
    f_ptr.argtypes = arg_types
    f_ptr.restype = ret_type
    return f_ptr


c_func_init_lexer = load_function_ptr("init_lexer", [], ctypes.c_void_p)
c_func_init_parser = load_function_ptr("init_parser", [], ctypes.c_void_p)
c_func_lexer_set_handle_bom = load_function_ptr("lexer_set_handle_bom", [ctypes.c_void_p, ctypes.c_bool], ctypes.c_int)
c_func_lexer_load_file = load_function_ptr("lexer_load_file", [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_int)
c_func_lexer_close_file = load_function_ptr("lexer_close_file", [ctypes.c_void_p], ctypes.c_int)
c_func_parser_load_lexer = load_function_ptr("parser_load_lexer", [ctypes.c_void_p, ctypes.c_void_p], ctypes.c_int)
c_func_parser_parse = load_function_ptr("parse", [ctypes.c_void_p], ctypes.c_void_p)
c_func_get_language = load_function_ptr("get_language", [ctypes.c_void_p], ctypes.c_char_p)
c_func_get_comments = load_function_ptr("get_comments", [ctypes.c_void_p], ctypes.c_void_p)
c_func_get_entries = load_function_ptr("get_entries", [ctypes.c_void_p], ctypes.c_void_p)
c_func_get_comments_size = load_function_ptr("get_comments_size", [ctypes.c_void_p], ctypes.c_ulong)
c_func_get_entries_size = load_function_ptr("get_entries_size", [ctypes.c_void_p], ctypes.c_ulong)
c_func_get_comment = load_function_ptr("get_comment", [ctypes.c_void_p, ctypes.c_ulong], ctypes.c_char_p)
c_func_get_entry = load_function_ptr("get_entry", [ctypes.c_void_p, ctypes.c_ulong], ctypes.c_void_p)
c_func_get_entry_key = load_function_ptr("get_entry_key", [ctypes.c_void_p], ctypes.c_char_p)
c_func_get_entry_value = load_function_ptr("get_entry_value", [ctypes.c_void_p], ctypes.c_char_p)
c_func_free_ptr = load_function_ptr("free_ptr", [ctypes.c_void_p], ctypes.c_int)


class PtrRef:
    def free_ptr(self):
        pass

    def get_ptr(self):
        pass


class Entry:
    key: str
    value: str

    def __init__(self, key: str, value: str):
        self.key = key
        self.value = value

    def __str__(self) -> str:
        return '<' + self.key + ": " + self.value + '>'

    def __repr__(self) -> str:
        return self.__str__()


class PdxLocalisation:
    language: str
    comments: list[str]
    entries: list[Entry]

    def __init__(self, language: str, comments: list[str], entries: list[Entry]):
        self.language = language
        self.comments = comments
        self.entries = entries

    def __str__(self) -> str:
        return "PdxLocalisation{language:" + self.language + ", \n comments: " + str(
            self.comments) + "\n entries:" + str(self.entries) + '}'

    def __repr__(self) -> str:
        return self.__str__()


class PdxLocalisationPtr(PtrRef):
    __ptr: ctypes.c_void_p

    def __init__(self, ref_ptr: ctypes.c_void_p):
        self.__ptr = ref_ptr

    def get_localisation(self) -> PdxLocalisation:
        pass

    def free_ptr(self):
        c_func_free_ptr(self.__ptr)

    def get_ptr(self):
        return self.__ptr


class Lexer(PtrRef):
    __c_ptr_lexer: ctypes.c_void_p

    def __init__(self, _ptr: ctypes.c_void_p):
        self.__c_ptr_lexer = _ptr

    def set_handle_bom(self, enable: bool):
        c_func_lexer_set_handle_bom(self.__c_ptr_lexer, ctypes.c_bool(enable))

    def load_file(self, filename: bytes):
        c_func_lexer_load_file(self.__c_ptr_lexer, ctypes.c_char_p(filename))

    def close_file(self):
        c_func_lexer_close_file(self.__c_ptr_lexer)

    def free_ptr(self):
        c_func_free_ptr(self.__c_ptr_lexer)

    def get_ptr(self):
        return self.__c_ptr_lexer


class Parser(PtrRef):
    __c_ptr_parser: ctypes.c_void_p

    def __init__(self, _ptr: ctypes.c_void_p):
        self.__c_ptr_parser = _ptr

    def load(self, lexer: Lexer):
        c_func_parser_load_lexer(self.__c_ptr_parser, lexer.get_ptr())

    def parse(self):
        ptr: PdxLocalisationPtr = self.parse_raw()
        language = get_language(ptr)
        comments = get_comments(ptr)
        entries = get_entries(ptr)
        ptr.free_ptr()
        return PdxLocalisation(language, comments, entries)

    def parse_raw(self) -> PdxLocalisationPtr:
        return PdxLocalisationPtr(c_func_parser_parse(self.__c_ptr_parser))

    def free_ptr(self):
        c_func_free_ptr(self.__c_ptr_parser)

    def get_ptr(self):
        return self.__c_ptr_parser


def create_lexer() -> Lexer:
    lexer_ptr = c_func_init_lexer()
    return Lexer(lexer_ptr)


def create_parser() -> Parser:
    parser_ptr = c_func_init_parser()
    return Parser(parser_ptr)


def get_language(ptr: PdxLocalisationPtr) -> str:
    language: bytes = c_func_get_language(ptr.get_ptr())
    return copy.deepcopy(str(language, "utf8"))


def get_comments(ptr: PdxLocalisationPtr) -> list[str]:
    comments_ptr: ctypes.c_void_p = c_func_get_comments(ptr.get_ptr())
    size: int = c_func_get_comments_size(comments_ptr)
    res: list[str] = []
    for index in range(0, size):
        comment: bytes = c_func_get_comment(comments_ptr, ctypes.c_ulong(index))
        try:
            res.append(copy.deepcopy(str(comment, "utf8")))
        except UnicodeDecodeError:
            print("Failed to convert comment due to UTF8 decode error, skip.")
    return res


def get_entries(ptr: PdxLocalisationPtr) -> list[Entry]:
    entries_ptr = ctypes.c_void_p = c_func_get_entries(ptr.get_ptr())
    size: int = c_func_get_entries_size(entries_ptr)
    res: list[Entry] = []
    for index in range(0, size):
        entry_ptr: ctypes.c_void_p = c_func_get_entry(entries_ptr, ctypes.c_ulong(index))
        key: bytes = c_func_get_entry_key(entry_ptr)
        value: bytes = c_func_get_entry_value(entry_ptr)
        key_utf = str(key, "utf8")
        try:
            value_utf = str(value, "utf8")
        except UnicodeDecodeError:
            print("Failed to convert value for '", key_utf, "' due to UTF8 decode error, skip.")
            continue
        res.append(copy.deepcopy(Entry(str(key, "utf8"), str(value, "utf8"))))
    return res

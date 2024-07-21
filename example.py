import os

import pypdx_yaml.pdx_yaml as pdx_yaml

if __name__ == "__main__":
    # create lexer and parser
    lexer = pdx_yaml.create_lexer()
    parser = pdx_yaml.create_parser()
    # set handle bom and load file
    lexer.set_handle_bom(True)
    lexer.load_file(b"./test.yml")
    # load the lexer
    parser.load(lexer)
    localisation_ptr = parser.parse_raw()
    print(pdx_yaml.get_language(localisation_ptr))
    print(pdx_yaml.get_comments(localisation_ptr))
    print(pdx_yaml.get_entries(localisation_ptr))
    localisation_ptr.free_ptr()
    lexer.close_file()
    # after a parse, the lexer is empty, so we need to
    # reload the lexer next time we use.
    lexer.load_file(b"./test.yml")
    localisation = parser.parse()
    print(localisation)

    # next we will test for the whole stellaris localisation file,
    # and the test file and test code will be deleted after this test.
    # pdx_dir = "./localisation"
    # for f1 in os.listdir(pdx_dir):
    #     p1 = os.path.join(pdx_dir, f1)
    #     if os.path.isdir(p1):
    #         for file in os.listdir(p1):
    #             path = os.path.join(p1, file)
    #             if os.path.isfile and file.endswith("yml"):
    #                 print("parse file: ", path)
    #                 # lexer.close_file()
    #                 lexer.load_file(bytes(path, "utf8"))
    #                 parser.parse_raw().free_ptr()
    #     elif f1.endswith("yml") and not f1.startswith("language"):
    #         print("parse file: ", p1)
    #         # lexer.close_file()
    #         lexer.load_file(bytes(p1, "utf8"))
    #         parser.parse_raw().free_ptr()
    lexer.free_ptr()
    parser.free_ptr()
    while True:
        line = input()
        if line == ":exit":
            break

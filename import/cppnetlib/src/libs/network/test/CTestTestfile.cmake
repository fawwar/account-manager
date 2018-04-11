# CMake generated Testfile for 
# Source directory: /home/enricohuang/cpp-netlib/libs/network/test
# Build directory: /home/enricohuang/cpp-netlib/libs/network/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(cpp-netlib-message_test "/home/enricohuang/cpp-netlib/tests/cpp-netlib-message_test")
ADD_TEST(cpp-netlib-message_transform_test "/home/enricohuang/cpp-netlib/tests/cpp-netlib-message_transform_test")
ADD_TEST(cpp-netlib-utils_thread_pool "/home/enricohuang/cpp-netlib/tests/cpp-netlib-utils_thread_pool")
SUBDIRS(uri)
SUBDIRS(http)

#!/usr/bin/env bash

header_template='#pragma once

namespace %NAMESPACE% {

class %CLASS_NAME% {
    public:
        %CLASS_NAME%();
        ~%CLASS_NAME%();
        %CLASS_NAME%(const %CLASS_NAME% &other);
        %CLASS_NAME%& operator=(const %CLASS_NAME% &rhs);

    private:

};

} /* namespace %NAMESPACE% */

'

source_template='#include "%SUB_FOLDER%/%CLASS_NAME%.hpp"

namespace %NAMESPACE% {

%CLASS_NAME%::%CLASS_NAME%() {

}

%CLASS_NAME%::~%CLASS_NAME%() {

}

%CLASS_NAME%::%CLASS_NAME%(const %CLASS_NAME% &other) {

}

%CLASS_NAME%& %CLASS_NAME%::operator=(const %CLASS_NAME% &rhs) {
    return *this;
}

} /* namespace %NAMESPACE% */

'

test_template='
#include <gtest/gtest.h>
#include "%SUB_FOLDER%/%CLASS_NAME%.hpp"

namespace %NAMESPACE% {

class %CLASS_NAME%Test : public ::testing::Test {
protected:
    %CLASS_NAME% instance; // Instance of the class being tested
};

TEST_F(%CLASS_NAME%Test, DefaultConstructor) {
    ASSERT_NO_THROW(%CLASS_NAME% new_instance;);
}

} /* namespace %NAMESPACE% */
'

help_text="
Usage: $0 <subfolder> <class_name>
This script creates a C++ class with header and source files in canonical form.

Arguments:
  subfolder   - The subdirectory under 'include' and 'src' where the class files will be placed.
  className  - The name of the class to create.

Canonical form:
  The generated class will have:
  - A default constructor.
  - A destructor.
  - A copy constructor.
  - A copy assignment operator.

Generated Files:
  - Header file (.hpp) placed in 'include/<subfolder>/'
  - Source file (.cpp) placed in 'src/<subfolder>/'
  - Test file (.cpp) placed in 'tests/<subfolder>/'

Each class will be generated with proper Doxygen documentation.

Examples:
  $0 logger LoggerManager
    Creates a C++ class named LoggerManager in the 'logger' subdirectory.
"

create_class() {
    local subfolder="$1"
    local namespace="$1"
    local class_name="$2"
    local header_path="include/${subfolder}/${class_name}.hpp"
    local source_path="src/${subfolder}/${class_name}.cpp"
    # local test_path="tests/${subfolder}/${class_name}Test.cpp"

    local processed_header=$(echo "${header_template}" | sed -e "s/%CLASS_NAME%/${class_name}/g" -e "s/%NAMESPACE%/${namespace}/g")
    local processed_source=$(echo "${source_template}" | sed -e "s/%CLASS_NAME%/${class_name}/g" -e "s/%SUB_FOLDER%/${subfolder}/g" -e "s/%NAMESPACE%/${namespace}/g")
    # local processed_test=$(echo "${test_template}" | sed -e "s/%CLASS_NAME%/${class_name}/g" -e "s/%SUB_FOLDER%/${subfolder}/g" -e "s/%NAMESPACE%/${namespace}/g")

    mkdir -p "include/${subfolder}" "src/${subfolder}" || {
        echo "Failed to create directories"
        exit 1
    }
    echo "Creating class files at ${header_path}, ${source_path}, and test file at ${test_path}"
    echo "${processed_header}" >"${header_path}"
    echo "${processed_source}" >"${source_path}"
    # echo "${processed_test}" >"${test_path}"
}

check_arguments() {
    if [ "$1" = "--help" ]; then
        echo "$help_text"
        exit 0
    fi

    if [ $# -ne 2 ]; then
        echo "Error: Expected 2 arguments, but got $#. For help, use: $0 --help"
        exit 1
    fi
}

main() {
    check_arguments "$@"
    create_class "$1" "$2"
}

main "$@"

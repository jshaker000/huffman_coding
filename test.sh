#!/bin/bash

[[ -z $TMP_FILE ]]   && TMP_FILE=$PWD/.HUFFMAN_CODING_REGRESSION_TMP_FILE
[[ -z $FAILS_FILE ]] && FAILS_FILE=$PWD/.HUFFMAN_CODING_REGRESSION_FAILS_FILE
[[ -z $BUILD_DIR ]]  && BUILD_DIR=$PWD/build
[[ -z $TEST_DIR ]]   && TEST_DIR=$PWD

rm -rf $FAILS_FILE
rm -rf $TMP_FILE

echo "Starting Huffman Coding Regression. Will try to encode, decode, and diff a group of files,"
echo "By default, tests the contents of this git repo."

HENC=$BUILD_DIR/huffman_encode
HDEC=$BUILD_DIR/huffman_decode

if [[ ! -s $HENC ]] || [[ ! -s $HDEC ]]
then
    echo "Error finding huffman_encode: $HENC or huffman_decode: $HDEC"
    echo "HUFFMAN CODING REGRESSION FAILURE!"
    exit -1
fi

for test_file in $(find $TEST_DIR)
do
    if [[ -s $test_file && -f $test_file  ]]
    then
        echo "Testing $test_file"
        $HENC $test_file 2>/dev/null | $HDEC > $TMP_FILE
        diff $test_file $TMP_FILE 1> /dev/null 2>/dev/null
        if [[ $? -ne 0 ]]
        then
            echo "FAILURE: $test_file" | tee -a $FAILS_FILE
        fi
        rm -rf $TMP_FILE
    fi
done

if [[ -s $FAILS_FILE ]]
then
    echo "HUFFMAN CODING REGRESSION FAILURE!"
    echo "Errors logged in $FAILS_FILE"
    exit -1;
else
    echo "HUFFMAN CODING REGRESSION SUCCESS!"
    exit 0;
fi

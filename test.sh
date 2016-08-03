#! /usr/bin/env bash

printf "\nTest Case 1\n"
./simpsh --verbose --rdonly foo --creat --wronly FOO \
         --command 3 4 2 tr a-z A-Z

printf "\n\nTest Case 2\n"
./simpsh --verbose --rdonly foo --command 3 1 2 tr a-z A-Z

printf "\n\nTest Case 3\n"
./simpsh --verbose --rdonly foo \
         --creat --wronly foo_sorted \
         --creat --wronly foo_error \
         --command 3 4 5 sort

printf "\n\nTest Case 4\n"
./simpsh --verbose --rdonly foo \
         --rdonly bar \
         --creat --append --wronly foo_bar \
         --command 3 5 2 sort \
         --command 3 5 2 cat bar -

printf "\n\nTest Case 5\n"
./simpsh --verbose --rdonly foo --rdonly oof --command 3 1 2 diff -y - oof

printf "\n\nTest Case 6\n"
./simpsh --verbose --rdonly foo --rdonly bar --pipe \
         --command 3 6 2 sort \
         --command 5 1 2 cat bar -

printf "\n\nTest Case 7\n"
./simpsh --verbose --rdonly foo --rdonly bar --pipe \
         --command 5 1 2 sort \
         --command 3 6 2 cat bar -

printf "\n\nTest Case 8\n"
./simpsh --verbose --wait \
         --rdonly foo \
         --rdonly bar \
         --creat --wronly foo_pipe8 \
         --pipe \
         --command 3 7 2 cat bar - \
         --command 6 5 1 tr a-z A-Z

printf "\n\nTest Case 9\n"
./simpsh --verbose --wait \
         --rdonly foo \
         --rdonly bar \
         --creat --wronly foo_pipe9 \
         --pipe \
         --command 6 5 1 tr a-z A-Z \
         --command 3 7 2 cat bar -

printf "\n\nTest Case 10\n"
./simpsh --verbose --rdonly foo --wronly bar --close 3 --close 4

printf "\n\nTest Case 11\n"
./simpsh --verbose --close 0 --close 1 --close 2

printf "\n\nTest Case 12\n"
./simpsh --verbose --abort

printf "\n\nTest Case 13\n"
./simpsh --verbose --ignore 11 --abort

printf "\n\nTest Case 14\n"
./simpsh --verbose --ignore 11 --abort --default 11 --abort

printf "\n\nTest Case 15\n"
./simpsh --verbose --ignore 11 --abort --default 11 --pause --abort

printf "\n\nTest Case 16\n"
./simpsh --verbose --catch 11 --abort

printf "\n\nTest Case 17\n"
./simpsh --verbose --ignore 11 --catch 11 --abort

printf "\n\nTest Case 18\n"
./simpsh --verbose --ignore 11 --abort --catch 11 --abort

printf "\n\nTest Case 19\n"
./simpsh --verbose --ignore 11 --abort --default 11 --catch 11 --abort

printf "\n\nTest Case 20\n"
./simpsh --verbose --wait \
         --rdonly foo \
         --rdonly bar \
         --rdonly FOOBAR_BASH \
         --pipe \
         --pipe \
         --creat --sync --rdwr FOOBAR_SIMPSH \
         --creat --append --wronly foo_finale_error \
         --pause \
         --ignore 11 \
         --abort \
         --default 11 \
         --command 4 7 11 cat foo - \
         --command 6 9 11 tr a-z A-Z \
         --command 8 10 11 sort

# foo -> 3
# bar -> 4
# FOOBAR_BASH -> 5
# pipe -> 6 7
# pipe -> 8 9
# FOOBAR_SIMPSH -> 10
# foo_finale_error -> 11
# cat foo bar | tr a-z A-Z | sort > FOOBAR_SIMPSH   Equivalent Bash Command

printf "\n\nTest Case 21\n"
./simpsh --verbose --wait --rdonly numbers \
         --profile --command 3 1 2 sed '1d; n; d' --pause

printf "\n\nTest Case 22\n"
./simpsh --verbose --wait --rdonly words \
         --profile --command 3 1 2 sort -Ru --pause

printf "\n\nTest Case 23\n"
./simpsh --verbose --wait --rdonly words --rdonly numbers \
         --profile --command 4 1 2 cat words --pause

printf "\nProfile Test Case 1 (bash): sed\n"
time sed '1d; n; d' numbers
sleep 3

printf "\nProfile Test Case 1 (execline): sed\n"
time execlineb sedTest
sleep 3

printf "\nProfile Test Case 1 (simpsh): sed\n"
./simpsh --rdonly numbers --profile --command 3 1 2 sed '1d; n; d'
sleep 3

printf "\nProfile Test Case 2 (bash): sort\n"
time sort -Ru words
sleep 3

printf "\nProfile Test Case 2 (execline): sort\n"
time execlineb sortTest
sleep 3

printf "\nProfile Test Case 2 (simpsh): sort\n"
./simpsh --rdonly words --profile --command 3 1 2 sort -Ru
sleep 3

printf "\nProfile Test Case 3 (bash): cat\n"
time cat words numbers
sleep 3

printf "\nProfile Test Case 3 (execline): cat\n"
time execlineb catTest
sleep 3

printf "\nProfile Test Case 3 (simpsh): cat\n"
./simpsh --rdonly words --rdonly numbers --profile --command 4 1 2 cat words -
sleep 3

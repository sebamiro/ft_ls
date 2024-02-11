#!/bin/bash

n_test=1

function test {
	local start=`perl -MTime::HiRes=time -e 'printf "%.9f\n", time'`
	ls_res=$(ls $1 2>&1)
	local end=`perl -MTime::HiRes=time -e 'printf "%.9f\n", time'`
	ls_time=$(echo "$end - $start" | bc -l)
	ls_ret=$?
	local start=`perl -MTime::HiRes=time -e 'printf "%.9f\n", time'`
	ft_res=$(../ft_ls $1 2>&1)
	local end=`perl -MTime::HiRes=time -e 'printf "%.9f\n", time'`
	ft_time=$(echo "$end - $start" | bc -l)
	ft_ret=$?
	diff_ret=$(diff <(echo $ls_res) <(echo $ft_res))
}

function log_error {
	printf FAIL
	echo "[ TEST $n_test: $1 ] ls_time: $ls_time ft_time: $ft_time" >> log
	echo "ls $2: [$ls_ret]" >> log
	echo $ls_res >> log
	echo "ft_ls $2: [$ft_ret]" >> log
	echo $ft_res >> log
	echo ----------- >> log
}

function do_test {
	printf "%-35s ] " "[ TEST $n_test: $1"
	test "$2"
	if [ $? -ne 0 ]; then
		log_error $1 $2
	else
		if [ $ls_ret -ne $ft_ret ]; then
			log_error $1 $2
		else
			printf OK
		fi
	fi
	echo " ls_time: $ls_time ft_time: $ft_time"
	n_test=$((n_test + 1))
}

rm -f log
do_test "current directory" ""
do_test "current directory (./)" "./"
do_test "root" "/"
do_test "absolute /bin" "/bin"
do_test "relative 2 behind" "../../"
do_test "error mising directory" "directory"
do_test "error file as paramter" "./tester.sh"
do_test "-l flag" "-l"
do_test "-R flag" "-R"
do_test "-R flag ~" "-R /Users/smiro/"
do_test "-a flag" "-a"
do_test "-r flag" "-r"
do_test "-t flag" "-t"
do_test "-l flag ~" "-l /Users/smiro/"
do_test "-lr flag ~" "-lr /Users/smiro/"
do_test "-lt flag ~" "-lt /Users/smiro/"
do_test "-la flag ~" "-la /Users/smiro/"
do_test "-lrat flag ~" "-lrat /Users/smiro/"
do_test "-lR flag ~" "-lR /Users/smiro/"
do_test "-Rlrat flag ~" "-Rlrat /Users/smiro/"

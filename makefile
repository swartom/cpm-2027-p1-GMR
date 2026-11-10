PHONY: test_case_builder_filename stc_builder runner gmr

gmr:
	gcc -g -pg -lm gmr.c -o testing

rungmr:
	./testing polygon.txt -v -t 2 -l 500

runner:
	gcc -g -O3 -pg main.c -o test

test_case_builder_filename:
	gcc -g instance_generator_v2.c -o test_builder 

stc_builder:
	gcc -O3 -g -pg stc.c -o stc_test

runtests:
	bash freecad.sh
	python analysis.py


clean:
	rm test testing stc_test test_builder

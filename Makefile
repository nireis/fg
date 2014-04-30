
all:
	g++ fg.cpp -o fg -O2 -Wall -g

run: all
	./fg

simple_plots:
	gnuplot -e "splot 'fc_data.txt' using 1:2:3 with dots, 'fc_solution_simple.txt' using 1:2:3 with lines" -p
	gnuplot -e "splot 'gc_data.txt' using 1:2:3 with dots, 'gc_solution_simple.txt' using 1:2:3 with lines" -p

backtracking_plots:
	gnuplot -e "splot 'fc_data.txt' using 1:2:3 with dots, 'fc_solution_backtracking.txt' using 1:2:3 with lines" -p
	gnuplot -e "splot 'gc_data.txt' using 1:2:3 with dots, 'gc_solution_backtracking.txt' using 1:2:3 with lines" -p

function_value_eval_plots:
	gnuplot -e "plot 'fc_no_alternative_value_over_eval.txt' using 2:1 with lines, 'fc_with_alternative_value_over_eval.txt' using 2:1 with lines" -p
	gnuplot -e "plot 'gc_no_alternative_value_over_eval.txt' using 2:1 with lines, 'gc_with_alternative_value_over_eval.txt' using 2:1 with lines" -p

metric_plots:
	gnuplot -e "splot 'fc_data.txt' using 1:2:3 with dots, 'fc_solution_metric.txt' using 1:2:3 with lines" -p
	gnuplot -e "splot 'gc_data.txt' using 1:2:3 with dots, 'gc_solution_metric.txt' using 1:2:3 with lines" -p

plots: run backtracking_plots function_value_eval_plots metric_plots


clean:
	rm -rf fg *.txt *.sh

set yrange [0:1]
plot 'ling_var.dat' using 1:2 title 'negative:L' with lines, 'ling_var.dat' using 1:3 title 'negative:H' with lines
pause 120

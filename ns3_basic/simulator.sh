#!/bin/bash

mkdir -p scratch/runs

rm throughput.dat
rm deliveryratio.dat
declare -a nNode=("20" "40" "60" "80" "100")
declare -a nFlow=("10" "20" "30" "40" "50")
declare -a nPacket=("100" "200" "300" "400" "500")
declare -a coverage=("1" "2" "3" "4" "5")

for i in ${nNode[@]}
do
    ./ns3 run "practice --nNodes=${i} --graph=0"
    echo nNodes $i 
done

cp throughput.dat scratch/runs/throughput_node.dat
cp deliveryratio.dat scratch/runs/deliveryratio_node.dat

# flows
rm throughput.dat
rm deliveryratio.dat

for i in ${nFlow[@]}
do
    ./ns3 run "practice --nFlows=${i} --graph=1"
    echo flows $i 
done

cp throughput.dat scratch/runs/throughput_flow.dat
cp deliveryratio.dat scratch/runs/deliveryratio_flow.dat

# packets
rm throughput.dat
rm deliveryratio.dat

for i in ${nPacket[@]}
do
    ./ns3 run "practice --nPackets=${i} --graph=2"
    echo packets $i 
done

cp throughput.dat scratch/runs/throughput_packet.dat
cp deliveryratio.dat scratch/runs/deliveryratio_packet.dat

# coverage
rm throughput.dat
rm deliveryratio.dat

for i in ${coverage[@]}
do
    ./ns3 run "practice --coverageArea=${i} --graph=3"
    echo coverage $i 
done

cp throughput.dat scratch/runs/throughput_coverage.dat
cp deliveryratio.dat scratch/runs/deliveryratio_coverage.dat



gnuplot -persist <<-EOFMarker
    set terminal png size 640,480
    set output "scratch/runs/throughput_vs_node.png"
    set xlabel "nodes"
    set ylabel "throughput"
    plot "scratch/runs/throughput_node.dat" using 1:2 title 'Throughput vs nodes' with linespoints

    set output "scratch/runs/delivery_ratio_vs_node.png"
    set xlabel "nodes"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_node.dat" using 1:2 title 'Delivery ratio vs nodes' with linespoints

    set output "scratch/runs/throughput_vs_flow.png"
    set xlabel "flows"
    set ylabel "throughput"
    plot "scratch/runs/throughput_flow.dat" using 1:2 title 'Throughput vs flows' with linespoints

    set output "scratch/runs/delivery_ratio_vs_flow.png"
    set xlabel "flows"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_flow.dat" using 1:2 title 'Delivery ratio vs flows' with linespoints

    set output "scratch/runs/throughput_vs_packet.png"
    set xlabel "packet"
    set ylabel "throughput"
    plot "scratch/runs/throughput_packet.dat" using 1:2 title 'Throughput vs packets' with linespoints

    set output "scratch/runs/delivery_ratio_vs_packet.png"
    set xlabel "packet"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_packet.dat" using 1:2 title 'Delivery ratio vs packets' with linespoints

    set output "scratch/runs/throughput_vs_coverage.png"
    set xlabel "coverage"
    set ylabel "throughput"
    plot "scratch/runs/throughput_coverage.dat" using 1:2 title 'Throughput vs coverage' with linespoints

    set output "scratch/runs/delivery_ratio_vs_coverage.png"
    set xlabel "coverage"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_coverage.dat" using 1:2 title 'Delivery ratio vs coverage' with linespoints
EOFMarker
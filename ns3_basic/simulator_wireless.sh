#!/bin/bash

mkdir -p scratch/runs_wireless

rm throughput.dat
rm deliveryratio.dat
declare -a nNode=("20" "40" "60" "80" "100")
declare -a nFlow=("10" "20" "30" "40" "50")
declare -a nPacket=("100" "200" "300" "400" "500")
declare -a speed=("5" "10" "15" "20" "25")

for i in ${nNode[@]}
do
    ./ns3 run "wireless_practice --nNodes=${i} --graph=0"
    echo nNodes $i 
done

cp throughput.dat scratch/runs_wireless/throughput_node.dat
cp deliveryratio.dat scratch/runs_wireless/deliveryratio_node.dat

# flows
rm throughput.dat
rm deliveryratio.dat

for i in ${nFlow[@]}
do
    ./ns3 run "wireless_practice --nFlows=${i} --graph=1"
    echo flows $i 
done

cp throughput.dat scratch/runs_wireless/throughput_flow.dat
cp deliveryratio.dat scratch/runs_wireless/deliveryratio_flow.dat

# packets
rm throughput.dat
rm deliveryratio.dat

for i in ${nPacket[@]}
do
    ./ns3 run "wireless_practice --nPackets=${i} --graph=2"
    echo packets $i 
done

cp throughput.dat scratch/runs_wireless/throughput_packet.dat
cp deliveryratio.dat scratch/runs_wireless/deliveryratio_packet.dat

# speed
rm throughput.dat
rm deliveryratio.dat

for i in ${speed[@]}
do
    ./ns3 run "wireless_practice --speed=${i} --graph=3"
    echo speed $i 
done

cp throughput.dat scratch/runs_wireless/throughput_speed.dat
cp deliveryratio.dat scratch/runs_wireless/deliveryratio_speed.dat



gnuplot -persist <<-EOFMarker
    set terminal png size 640,480
    set output "scratch/runs_wireless/throughput_vs_node.png"
    set xlabel "nodes"
    set ylabel "throughput"
    plot "scratch/runs_wireless/throughput_node.dat" using 1:2 title 'Throughput vs nodes' with linespoints

    set output "scratch/runs_wireless/delivery_ratio_vs_node.png"
    set xlabel "nodes"
    set ylabel "delivery ratio"
    plot "scratch/runs_wireless/deliveryratio_node.dat" using 1:2 title 'Delivery ratio vs nodes' with linespoints

    set output "scratch/runs_wireless/throughput_vs_flow.png"
    set xlabel "flows"
    set ylabel "throughput"
    plot "scratch/runs_wireless/throughput_flow.dat" using 1:2 title 'Throughput vs flows' with linespoints

    set output "scratch/runs_wireless/delivery_ratio_vs_flow.png"
    set xlabel "flows"
    set ylabel "delivery ratio"
    plot "scratch/runs_wireless/deliveryratio_flow.dat" using 1:2 title 'Delivery ratio vs flows' with linespoints

    set output "scratch/runs_wireless/throughput_vs_packet.png"
    set xlabel "packet"
    set ylabel "throughput"
    plot "scratch/runs_wireless/throughput_packet.dat" using 1:2 title 'Throughput vs packets' with linespoints

    set output "scratch/runs_wireless/delivery_ratio_vs_packet.png"
    set xlabel "packet"
    set ylabel "delivery ratio"
    plot "scratch/runs_wireless/deliveryratio_packet.dat" using 1:2 title 'Delivery ratio vs packets' with linespoints

    set output "scratch/runs_wireless/throughput_vs_speed.png"
    set xlabel "speed"
    set ylabel "throughput"
    plot "scratch/runs_wireless/throughput_speed.dat" using 1:2 title 'Throughput vs speed' with linespoints

    set output "scratch/runs_wireless/delivery_ratio_vs_speed.png"
    set xlabel "speed"
    set ylabel "delivery ratio"
    plot "scratch/runs_wireless/deliveryratio_speed.dat" using 1:2 title 'Delivery ratio vs speed' with linespoints
EOFMarker
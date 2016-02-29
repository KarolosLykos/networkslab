set ns [new Simulator]

set nt [open trace.tr w]
set nf [open prob1.nam w]

$ns namtrace-all $nf
$ns trace-all $nt

proc finish {} {
global ns nf
$ns flush-trace
close $nf
exit 0
}

#$ns rtproto DV 

set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
set n6 [$ns node]

$ns duplex-link $n0 $n1 1.5Mb 10ms DropTail
$ns duplex-link $n1 $n2 1.5Mb 10ms DropTail
$ns duplex-link $n2 $n3 1.5Mb 10ms DropTail
$ns duplex-link $n3 $n4 1.5Mb 10ms DropTail
$ns duplex-link $n4 $n5 1.5Mb 10ms DropTail
$ns duplex-link $n5 $n6 1.5Mb 10ms DropTail
$ns duplex-link $n6 $n0 1.5Mb 10ms DropTail

set udp0 [new Agent/UDP]
$ns attach-agent $n0 $udp0 

set cbr [new Application/Traffic/CBR]
$cbr set interval_ 0.005
$cbr set packetSize_ 500 
$cbr attach-agent $udp0

set null [new Agent/Null]
$ns attach-agent $n3 $null

$ns connect $udp0 $null

$ns at 0.0 "$cbr start"
$ns rtmodel-at 1.0 down $n1 $n2
$ns rtmodel-at 2.0 up $n1 $n2
$ns at 4.5 "$cbr stop"

$ns at 4.6 "finish"
$ns run

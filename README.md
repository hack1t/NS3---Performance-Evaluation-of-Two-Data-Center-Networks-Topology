# NS3---Performance-Evaluation-of-Two-Data-Center-Networks-Topology

Software Requirement:
Development language: c++

            1)ns-3.23
            
            2)ubuntu 14.04

-------------------------------------------------------------------------------------
Implementation :
 
 Step 1: Implemented Hyper cube  with 16 nodes of DCN

 Step 2: Next implemented  torus  with 16 nodes of DCN
 
 Step 3:link contention may still occur influencing the measurement of connection throughput,
         latency and loss rate which is analysed in the simulation results
 
Step 4: Resultant graph can be generated using gnuplot
---------------------

Running procedure :

---------------------

step 1: copy the hyper.txt,node_coordinates.txt,torusnodes.txt,hypercube.txt files paste into  ns-3.23/

Step 2: copy the two files in Hypercube-DNS-topology.cc,torus-3d.cc,torus-3d.h paste into scratch folder

Step 3: run this 2 files Hypercube-DNS-topology.cc and torus-3d.cc (for eg: ./waf --run Hypercube-DNS-topology  and ./waf --run torus-3d)

Step 4: run NetAnim (use command :./NetAnim [location is :ns-allinone-3.23/netanim-3.106]).After Browse and give the HyperCube16node.anim.xml and torus-3d-DNS.anim.xml and-then run

             

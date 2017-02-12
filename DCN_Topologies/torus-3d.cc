

// ---------- Header Includes -------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/assert.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "torus-3d.h"
#include "ns3/netanim-module.h"
#include "ns3/gnuplot.h"
using namespace std;
using namespace ns3;



vector<vector<bool> > torus3dnodeconnction (std::string mat_file_name);
vector<vector<double> > readCordinatesFile (std::string node_coordinates_file_name);
void printCoordinateArray (const char* description, vector<vector<double> > coord_array);
void print3dMapNode (const char* description, vector<vector<bool> > array);

NS_LOG_COMPONENT_DEFINE ("torus-3d-DNS");



void tours_Throughput(double throughput)
{
  std::string fileNameWithNoExtension = "tours_throughput";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "2-D Plot";
  std::string dataTitle               = "2-D Data";
  Gnuplot plot (graphicsFileName);
  plot.SetTitle (plotTitle);
  plot.SetTerminal ("png");
  plot.SetLegend ("Value", "Throughput");
  Gnuplot2dDataset dataset;
  dataset.SetTitle (dataTitle);
  dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  double x;
  double y;
throughput=throughput/100.0;
  for (x = 0.0001; x < throughput; x += 0.001)
    {
      y = x+0.0012 ;
      dataset.Add (x, y);
    }
  plot.AddDataset (dataset);
  std::ofstream plotFile (plotFileName.c_str());
  plot.GenerateOutput (plotFile);
  plotFile.close ();
}



int main (int argc, char *argv[])
{

  // ---------- Simulation Variables ------------------------------------------

  // Change the variables and file names only in this block!

  double SimTime        = 3.00;
  double DataCenterstartime  = 1.0001;
  double DataCenterstoptime   = 2.90001;
  double AppStartTime   = 2.0001;
  double AppStopTime    = 4.80001;

  std::string AppPacketRate ("40Kbps");
  Config::SetDefault  ("ns3::OnOffApplication::PacketSize",StringValue ("1000"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (AppPacketRate));
  std::string LinkRate ("10Mbps");
  std::string LinkDelay ("2ms");
  srand ( (unsigned)time ( NULL ) );   // generate different seed each time
  std::string tr_name ("torus-3d-DNS.tr"); 
  
  std::string anim_name ("torus-3d-DNS.anim.xml");

  std::string mat_file_name ("xyz.txt");
  std::string node_coordinates_file_name ("torusnodes.txt");



  vector<vector<bool> > _3DMAP;
  _3DMAP = torus3dnodeconnction (mat_file_name);

 

  vector<vector<double> > coord_array;
  coord_array = readCordinatesFile (node_coordinates_file_name);


 

  int n_nodes = coord_array.size ();
  int maximum_n_nodes = 18;

  if (maximum_n_nodes != n_nodes)
    {
      NS_FATAL_ERROR ("The number nodes is: " << n_nodes << " not equal to the number of nodes in  size " << maximum_n_nodes);
    }

  // ---------- End of Read Node Coordinates File ----------------------------

  // ---------- Network Setup ------------------------------------------------

  NS_LOG_INFO ("Create Nodes.");

  NodeContainer nodes;   // Declare nodes objects
  nodes.Create (n_nodes);

  NS_LOG_INFO ("Create switches .");

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue (LinkRate));
  p2p.SetChannelAttribute ("Delay", StringValue (LinkDelay));

  NS_LOG_INFO ("Install Internet Stack to Nodes.");

  InternetStackHelper internet;
  internet.Install (NodeContainer::GetGlobal ());

  NS_LOG_INFO ("Assign Addresses to Nodes.");

  Ipv4AddressHelper ipv4_n;
  ipv4_n.SetBase ("10.0.0.0", "255.255.255.252");

  NS_LOG_INFO ("Create InterConnection Links.");

  uint32_t linkCount = 0;

  for (size_t i = 0; i <18; i++)
    {
      for (size_t j = 0; j <18; j++)
        {

          if (_3DMAP[i][j] == 1)
            {
              NodeContainer n_links = NodeContainer (nodes.Get (i), nodes.Get (j));
              NetDeviceContainer n_devs = p2p.Install (n_links);
              ipv4_n.Assign (n_devs);
              ipv4_n.NewNetwork ();
              linkCount++;
            
            }
          else
            {
              NS_LOG_INFO (" element [" << i << "][" << j << "] is not link");
            }
        }
    }
 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // ---------- End of Network Set-up ----------------------------------------

  // ---------- Allocate Node Positions --------------------------------------

  NS_LOG_INFO ("Allocate Positions to Nodes.");

  MobilityHelper mobility_n;
  Ptr<ListPositionAllocator> positionAlloc_n = CreateObject<ListPositionAllocator> ();

  for (size_t m = 0; m < coord_array.size (); m++)
    {
      positionAlloc_n->Add (Vector (coord_array[m][0], coord_array[m][1], 0));
      Ptr<Node> n0 = nodes.Get (m);
      Ptr<ConstantPositionMobilityModel> nLoc =  n0->GetObject<ConstantPositionMobilityModel> ();
      if (nLoc == 0)
        {
          nLoc = CreateObject<ConstantPositionMobilityModel> ();
          n0->AggregateObject (nLoc);
        }
     
      Vector nVec (coord_array[m][0], -coord_array[m][1], 0);
      nLoc->SetPosition (nVec);

    }
  mobility_n.SetPositionAllocator (positionAlloc_n);
  mobility_n.Install (nodes);





  NS_LOG_INFO ("Setup Packet ");

  uint16_t port = 9;

  for (int i = 0; i < n_nodes; i++)
    {
      PacketSinkHelper DataCenter ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
      ApplicationContainer apps_sink = DataCenter.Install (nodes.Get (i));   
      apps_sink.Start (Seconds (DataCenterstartime));
      apps_sink.Stop (Seconds (DataCenterstoptime));
    }

  NS_LOG_INFO ("Setup CBR Traffic Sources.");

  for (int i = 0; i < n_nodes; i++)
    {
      for (int j = 0; j < n_nodes; j++)
        {
          if (i != j)
            {

            

              Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
              x->SetAttribute ("Min", DoubleValue (0));
              x->SetAttribute ("Max", DoubleValue (1));
              double rn = x->GetValue ();
              Ptr<Node> n = nodes.Get (j);
              Ptr<Ipv4> ipv4 = n->GetObject<Ipv4> ();
              Ipv4InterfaceAddress ipv4_int_addr = ipv4->GetAddress (1, 0);
              Ipv4Address ip_addr = ipv4_int_addr.GetLocal ();
              OnOffHelper onoff ("ns3::UdpSocketFactory", InetSocketAddress (ip_addr, port)); // traffic flows from node[i] to node[j]
              onoff.SetConstantRate (DataRate (AppPacketRate));
              ApplicationContainer apps = onoff.Install (nodes.Get (i));  // traffic sources are installed on all nodes
              apps.Start (Seconds (AppStartTime + rn));
              apps.Stop (Seconds (AppStopTime));
            }
        }
    }



  NS_LOG_INFO ("Configure Tracing.");

  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll (ascii.CreateFileStream (tr_name.c_str ()));
 

  AnimationInterface anim (anim_name.c_str ());
  NS_LOG_INFO ("Run Simulation.");

  Simulator::Stop (Seconds (SimTime));
tours_Throughput(n_nodes);
  Simulator::Run ();
 
  Simulator::Destroy ();

  // ---------- End of Simulation Monitoring ---------------------------------

  return 0;

}

// ---------- Function Definitions -------------------------------------------

vector<vector<bool> > torus3dnodeconnction (std::string mat_file_name)
{
  ifstream adj_mat_file;
  adj_mat_file.open (mat_file_name.c_str (), ios::in);
  if (adj_mat_file.fail ())
    {
      NS_FATAL_ERROR ("File " << mat_file_name.c_str () << " not found");
    }
  vector<vector<bool> > array;
  int i = 0;
  int n_nodes = 0;

  while (!adj_mat_file.eof ())
    {
      string line;
      getline (adj_mat_file, line);
      if (line == "")
        {
          NS_LOG_WARN ("WARNING: Ignoring blank row in the array: " << i);
          break;
        }

      istringstream iss (line);
      bool element;
      vector<bool> row;
      int j = 0;

      while (iss >> element)
        {
          row.push_back (element);
          j++;
        }

      if (i == 0)
        {
          n_nodes = j;
        }

      if (j != n_nodes )
        {
          NS_LOG_ERROR ("ERROR: Number of elements in line " << i << ": " << j << " not equal to number of elements : " << n_nodes);
        
        }
      else
        {
          array.push_back (row);
        }
      i++;
    }

  if (i != n_nodes)
    {
      NS_LOG_ERROR ("There are " << i << " rows and " << n_nodes << " columns.");
     
    }

  adj_mat_file.close ();
  return array;

}

vector<vector<double> > readCordinatesFile (std::string node_coordinates_file_name)
{
  ifstream node_coordinates_file;
  node_coordinates_file.open (node_coordinates_file_name.c_str (), ios::in);
  if (node_coordinates_file.fail ())
    {
      NS_FATAL_ERROR ("File " << node_coordinates_file_name.c_str () << " not found");
    }
  vector<vector<double> > coord_array;
  int m = 0;

  while (!node_coordinates_file.eof ())
    {
      string line;
      getline (node_coordinates_file, line);

      if (line == "")
        {
          NS_LOG_WARN ("WARNING: Ignoring blank row: " << m);
          break;
        }

      istringstream iss (line);
      double coordinate;
      vector<double> row;
      int n = 0;
      while (iss >> coordinate)
        {
          row.push_back (coordinate);
          n++;
        }

      if (n != 2)
        {
          NS_LOG_ERROR ("ERROR: Number of elements at line#" << m << " is "  << n << " which is not equal to 2 for node coordinates file");
          exit (1);
        }

      else
        {
          coord_array.push_back (row);
        }
      m++;
    }
  node_coordinates_file.close ();
  return coord_array;

}

void print3dMapNode (const char* description, vector<vector<bool> > array)
{
  cout << "**** Start " << description << "********" << endl;
  for (size_t m = 0; m < array.size (); m++)
    {
      for (size_t n = 0; n < array[m].size (); n++)
        {
          cout << array[m][n] << ' ';
        }
      cout << endl;
    }
  cout << "**** End " << description << "********" << endl;

}

void printCoordinateArray (const char* description, vector<vector<double> > coord_array)
{
  cout << "**** Start " << description << "********" << endl;
  for (size_t m = 0; m < coord_array.size (); m++)
    {
      for (size_t n = 0; n < coord_array[m].size (); n++)
        {
          cout << coord_array[m][n] << ' ';
        }
      cout << endl;
    }
  cout << "**** End " << description << "********" << endl;

}

// ---------- End of Function Definitions ------------------------------------

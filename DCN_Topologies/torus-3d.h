
#ifndef NocTorus3D_H_
#define NocTorus3D_H_


#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/trace-helper.h"
#include "ns3/object-factory.h"

namespace ns3
{

  class NocTorus3D
  {
  public:

    static TypeId
    GetTypeId();

    Torus3D ();

    virtual
    ~Torus3D ();

    NetDeviceContainer
    Install (NodeContainer nodes);



   
  private:

    /**
     * how many nodes the 3D torus will have on one horizontal and vertical line
     */
    uint32_t m_hSize;
    uint32_t m_vSize;

  };

} // namespace ns3

#endif /* Torus3D_H_ */

# E-C-ITS
## Extended Cooperative Intelligent Transport Systems
Connected and Autonomous Vehicles (CAVs) have their attack surface widened. The crucial environment perception process is based on two sources of input: in-vehicle and external. An attacker’s might target physical sensors on the Connected and Autonomous Vehicle (CAV) nodes, for example, conducting a blinding attack of the cameras used or spoofing signals to make an obstacle (dis)appear from the node’s sight. If physical access is viable, an attacker might physically tamper with the sensors, which could lead to incorrect readings. These attacks mostly translate into a security/safety threat or Denial of Service (DoS) attack, with incorrect readings being passed on to the environment perception process. Most of these attacks are conducted on a single-node basis, so large-scale attacks successfully targeting the majority of nodes in a given geographic area is unlikely. This research proposes a consensus mechanism that significantly increases the costs of a successful attack. Today’s in-development protocols and the Vehicle to Everything (V2X) concept contemplate the frequent data sharing amongst nodes. The proposed mechanism builds on top of such: A consensus protocol for nodes to agree on what is indeed on the road, assuming the number of nodes participating is greater than three and the attacker does not control the majority of them.

Author: Patricia Rodrigues 
2021

A special thank you to the team of researchers at Computer and Communication Systems (CCS) for making the [Open Source Experimental and Prototyping Platform Supporting ETSI ITS-G5 (OpenC2X)](https://www.ccs-labs.org/software/openc2x/) available for community use.

I performed modifications in some of the OpenC2X project so that I could integrate ECITS on it. Do check out their work!

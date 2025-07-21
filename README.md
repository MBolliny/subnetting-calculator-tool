This is a C project that automates the process of subnetting and IP address assignment for multiple networks based on their host requirements. It was created to solve a real exam problem from the "Fondamenti di Comunicazione e Internet" course (Politecnico di Milano).

Features
- Input a list of networks and their required number of hosts.
- Calculates the number of bits required for each subnet.
- (Work in progress) Groups networks with the same host requirements.
- (Upcoming) Assigns IPs to each network starting from a base IP and netmask.
- Designed to simulate VLSM subnetting (Variable Length Subnet Masking).

Input
- Network name (string).
- Number of hosts (int).
- Starting IP address (planned).
- Netmask in bits (planned).

Calculation
* Bits required = log2(hosts + 2) (to include reserved addresses).
* Subnet count and subnet bits derived from IP length and netmask.

Usage
1. Compile the code: gcc -o subnetting subnetting.c -lm
2. Run: ./subnetting
3. Follow prompts to enter networks and host counts.

Structure
- networkTable_t: stores name, host count, and bits required.
- group_t and groupMember_t: planned for grouping by bit size.
- IPlist_t: planned for storing assigned IPs and netmasks.

To Do
- Finish grouping logic (find_groups)
- Implement IP assignment and subnet calculation
- Add support for IPv4 parsing and printing in dotted format

# CHANGELOG

## Version 1.0.0

### Implemented Functionality
- Developed an L4 scanner capable of scanning ports.
- Supports both TCP and UDP port scanning.

### Known Limitations
1. **UDP Scanning Anomaly**: 
    - Occasionally, running multiple UDP scans in quick succession produced inconsistent results. This issue could not be reliably replicated.
2. **Lack of Multithreading**: 
    - The scanner currently operates in a single-threaded mode. Multithreading was planned but could not be implemented due to time constraints.
3. **Interface and Address Specification**:  
    - If the user specifies an interface (e.g., `eth0`) or an outgoing address, the behavior may vary.  
        - For `localhost` with IPv4, the scanner will work but may not receive a response, leading to inconsistent results.  
        - For IPv6, the scanner might fail to send packets.  
    - These issues might be considered user errors due to improper configuration.
    - **fix** for now, I am forcing the interface to be lo, in case you specify target localhost

### Notes
- As of now, the scanner is believed to function correctly under normal usage conditions.
- Documented limitations are provided to ensure transparency.
- No other known issues have been identified.
- Future updates may address the above limitations.
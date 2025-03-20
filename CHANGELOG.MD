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

### Notes
- As of now, the scanner is believed to function correctly under normal usage conditions.
- Documented limitations are provided to ensure transparency.
- No other known issues have been identified.
- Future updates may address the above limitations.
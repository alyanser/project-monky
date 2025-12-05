# Changelog - Project Monky

## Fork Information
**Forked from:** Multi Theft Auto: San Andreas (MTA:SA)
**Fork Date:** October 2025
**License:** GPLv3

---

## Major Changes from MTA:SA

### Network & Authentication
- Implemented custom authentication protocol between client and server
- Removed connections to MTA master servers
- Blocked server IP exposure via GetServerIP()

### Branding & User Interface
- Complete rebranding to Project Monky across client, server, and installer
- Redesigned main menu with custom Discord and Website buttons
- Custom splash screen and background imagery
- Removed MTA news feed and version displays
- Separate Windows registry paths to avoid conflicts with MTA installations

### Client Modifications
- Changed default client settings (FOV, graphics options)
- Removed MTA auto-update system, redirects to Project Monky website

### Server Modifications
- Modified connection protocol to work with custom client

---

## Notes
This is an independent fork and creates a separate multiplayer network from MTA:SA. Clients and servers are not compatible with official MTA:SA infrastructure.

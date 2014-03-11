/*
 * Copyright (C) 2014 Joel Schlosser All Rights Reserved.
 *
 * This file is part of smudge.
 *
 * smudge is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * smudge is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with smudge.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TELNET_H
#define TELNET_H

#define TELNET_SE 240 // End of subnegotiation parameters.
#define TELNET_NOP 241 // No operation.
#define TELNET_DATA_MARK 242 // The data stream portion of a Synch. This should always be
                             // accompanied by a TCP Urgent notification.
#define TELNET_BREAK 243 // NVT character BRK.
#define TELNET_IP 244 // The function IP (interrupt process).
#define TELNET_AO 245 // The function AO (abort output).
#define TELNET_AYT 246 // The function AYT (are you there).
#define TELNET_EC 247 // The function EC (erase character).
#define TELNET_EL 248 // The function EL (erase line).
#define TELNET_GA 249 // The GA (go ahead) signal.
#define TELNET_SB 250 // Indicates that what follows is subnegotiation of the indicated option.
#define TELNET_WILL 251 // Indicates the desire to begin performing, or confirmation that
                        // you are now performing, the indicated option.
#define TELNET_WONT 252 // Indicates the refusal to perform, or continue performing, the
                        // indicate option.
#define TELNET_DO 253 // Indicates the request that the other party perform, or
                      // confirmation that you are expecting the other party to perform, the
                      // indicated option.
#define TELNET_DONT 254 // Indicates the demand that the other party stop performing,
                        // or confirmation that you are no longer expecting the other party
                        // to perform, the indicated option.
#define TELNET_IAC 255 // Interpret As Command. Indicates the start of a telnet option
                       // negotiation.

#define TELNET_MAX_COMMAND_SIZE 64 // Max size of command supported. It's capped arbitrarily
                                   // here. The only command that spans over three total bytes
                                   // is sub-negotiation.

// State machine for extracting telnet commands. Once the beginning of
// a command is seen, subsequent data is stored until the end of
// the command or until an arbitrary max length. The ready flag will
// then be set to indicate that the command should be handled externally.
typedef struct {
    unsigned char cmd[TELNET_MAX_COMMAND_SIZE]; // Stores the full telnet command in progress.
    int cmd_len; // The length of the telnet command in progress
    int cmd_ready; // Set to 1 when a command is ready. Note that no additional state changes
                   // will be made until the command is handled and this is set back to 0.
} telnet;

// Constructor/destructor.
telnet *telnet_create(void);
void telnet_destroy(telnet *tel);

// Member functions.
void telnet_clear_cmd(telnet *tel);
int telnet_update(telnet *tel, unsigned char byte); // Returns 1 if the telnet state changed; 0 otherwise.

#endif

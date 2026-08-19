*This project has been created as part of the 42 curriculum by dgomez-b, ifluxa-c.*

# ft_irc

## Description

**ft_irc** is a custom implementation of an **IRC (Internet Relay Chat) server**, written from scratch in **C++98**, without the use of any external or Boost libraries.

The goal of the project is to understand and implement one of the standard protocols that govern communication on the Internet, by building a server capable of handling multiple clients at the same time, using a **single `poll()` call** to multiplex all I/O operations (listening socket, client reads, and client writes) in a fully **non-blocking** way. No forking is used.

The server can be used with any real IRC client (our reference client is **HexChat**), and supports the core features of the protocol:

- Client authentication with a server password, nickname and username.
- Joining and leaving channels, and exchanging public messages within a channel as well as private messages between clients.
- A distinction between **regular users** and **channel operators**.
- Operator-only commands:
  - `KICK` — eject a client from a channel.
  - `INVITE` — invite a client to a channel.
  - `TOPIC` — view or change a channel's topic.
  - `MODE` — change a channel's mode:
    - `i` — invite-only channel.
    - `t` — restrict `TOPIC` changes to operators.
    - `k` — set/remove a channel key (password).
    - `o` — give/take operator privilege.
    - `l` — set/remove the channel's user limit.

The project is organized around a small class hierarchy: generic, reusable server/client base classes (`AServer`, `AClient`, `AServerClient`, `Socket`) handle the low-level networking and connection management, while IRC-specific classes (`IRCServer`, `IRCClient`, `Channel`, `IRCCommand`) implement the protocol logic (command parsing, channels, and the IRC commands themselves) on top of that foundation.

## Instructions

### Compilation

```bash
make        # builds the ircserv executable
make clean  # removes object files
make fclean # removes object files and the executable
make re     # fclean + all
```

The code is compiled with `c++`, using the flags `-Wall -Wextra -Werror -std=c++98`.

### Execution

```bash
./ircserv <port> <password>
```

- `port`: the port on which the server will listen for incoming IRC connections.
- `password`: the connection password that any IRC client must provide to authenticate (`PASS` command).

### Connecting with a client

Once the server is running, connect with any IRC client (e.g. HexChat), pointing it to `127.0.0.1` (or the server's host) on the chosen port, and supplying the server password when prompted. From there you can set a nickname/username, join channels, and send messages as with any standard IRC server.

## Resources

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- `man` pages for `socket`, `poll`, `bind`, `listen`, `accept`, `send`, `recv`, `fcntl`
- HexChat documentation, used as our reference client during development and testing

**Use of AI**: AI assistance was used at two points in the project. First, during the initial planning stage, to clarify doubts about the IRC protocol and to help think through the class design before starting to code. Second, once the mandatory part was finished, to help design and run test scenarios against the server. All AI-assisted output was reviewed and validated by the team before being used.

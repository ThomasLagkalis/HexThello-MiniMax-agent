# UC Hexthello v0.98b

## Authors
- **Base Code:** Ioannis Skoulakis  
- **Agent Extensions:** Thomas Lagkalis  
- **Affiliation:** Technical University of Crete, Greece  

---

## Requirements
- Any Linux distribution

## Compilation
To build the project, use the following commands:

- `make [all]` – Builds both client and server
- `make client` – Builds only the client
- `make server` – Builds only the server

## Execution
Run the necessary components using:

- Start the GUI server:  
  ```bash
  ./guiServer

- Or the default CLI server:
  ```bash
  ./server [-p port] [-g number_of_games] [-s (swap color after each game)]

- Defualt agent (minimax with alpha-beta pruning and all the heuristics):
  ```bash
  ./client [-i ip] [-p port]

- Only minimax agent (wihout alpha-beta pruning):
  ```bash
  ./client -m [-i ip] [-p port]


## Agent Extensions

- Minimax agent with alpha-beta pruning 
- Move ordering on minimax algorithm
- Heuristics on evaluation function 
- Iterative Depening Search (IDS)

For a more technical overview of the agent check the project [report](https://github.com/ThomasLagkalis/HexThello-MiniMax-agent/blob/main/report.pdf).

# Advanced Tic Tac Toe Game

## Project Overview

This is an advanced Tic Tac Toe game developed in C++ that incorporates user authentication, personalized game history, and an intelligent AI opponent . The game supports both player-vs-player and player-vs-AI modes, allowing users to log in, track their game history, and analyze past games . The project employs best practices in software engineering, including secure user management, rigorous testing, and professional version control workflows .

## Features

### Core Gameplay
- **Interactive 3x3 Tic Tac Toe Grid**: Classic gameplay with modern enhancements allowing two players and a player vs AI to take turns marking the grid 
- **Player vs Player Mode**: Two human players can compete against each other
- **Player vs AI Mode**: Challenge an intelligent AI opponent powered by advanced algorithms 
- **Win/Tie Detection**: Automatic game state evaluation and rule checking after each move 

### AI Opponent
- **Minimax Algorithm**: Strategic AI decision-making with alpha-beta pruning optimization for intelligent gameplay 
- **Challenging Gameplay**: AI makes strategic moves based on current game state 
- **Advanced Algorithms**: Alternative AI techniques can be implemented for enhanced opponent behavior 

### User Management
- **Secure Authentication**: User registration and login system with password hashing for secure credential storage 
- **User Profiles**: Individual player accounts with personalized settings and profile management 
- **Session Management**: Secure user sessions throughout gameplay to keep users logged in 

### Game History & Analytics
- **Personalized Game History**: Complete record of all played games with details stored securely in efficient format 
- **Game Replay**: Review and replay past games from history for analysis 
- **Performance Tracking**: Statistics and analytics for player improvement

### User Interface
- **Graphical User Interface**: User-friendly GUI that displays the Tic Tac Toe board with click-based interaction 
- **Interactive Board**: Click-based game interaction for seamless gameplay 
- **Login/Registration Forms**: GUI elements for managing user sessions including authentication forms 
- **Game History Views**: Easy access to past game records through dedicated interface elements 

## Technical Specifications

### Technologies Used
- **Programming Language**: C++ with Object-Oriented Programming principles 
- **GUI Framework**: Qt or similar C++ GUI libraries suitable for the platform 
- **Database**: SQLite or lightweight database system for managing user data and game histories 
- **Testing Framework**: Google Test for comprehensive unit and integration testing 
- **Version Control**: Git with GitHub as remote repository for effective version control 

### Data Structures
- **Trees**: Implementation of minimax algorithm for AI opponent using tree structures 
- **Hash Tables**: Efficient data storage and retrieval for various system components 
- **Stacks**: Various system components and game state management 
- **Custom Data Structures**: Additional structures for file storage with customized formats 

### Security Features
- **Password Hashing**: Secure credential storage using industry-standard hashing algorithms 
- **Session Security**: Protected user sessions and data integrity throughout gameplay 
- **Input Validation**: Comprehensive validation to prevent security vulnerabilities

## Installation & Setup

### Prerequisites
- C++ compiler (GCC 9.0+ or equivalent)
- Qt framework (version 5.15 or later) or similar GUI libraries 
- SQLite3 development libraries for database functionality 
- Google Test framework for testing capabilities 
- CMake (version 3.16 or later)


### Core Components

#### `/core/include/`
The core module contains the fundamental game logic and algorithms that power the Tic Tac Toe experience:

- **AIPlayer.h**: Implements the minimax algorithm with alpha-beta pruning for strategic AI gameplay, enabling the computer opponent to make intelligent decisions based on game state analysis 
- **GameBoard.h**: Manages the 3x3 game grid mechanics, including move validation, board state tracking, and win condition detection after each player move 
- **GameHistory.h**: Handles secure storage and retrieval of personalized game sessions, allowing players to maintain detailed records of their gameplay history 
- **GameStateStack.h**: Manages game state transitions using stack data structures, providing functionality for undo operations and state management throughout gameplay 
- **UserManager.h**: Provides comprehensive user authentication, registration, and session management with secure password hashing for credential storage 

#### `/GUI/`
The graphical user interface module delivers an intuitive and interactive gaming experience:

- **TicTacToeGUI.h**: Main graphical interface header that defines the user interaction framework, including click-based controls for board interaction and menu navigation 
- **resources/styles.qss**: Qt stylesheet definitions that ensure consistent visual design and professional appearance across all GUI components 

#### `/src/`
The source implementation directory contains the executable code that brings the application to life:

- **main.cpp**: Application entry point that initializes the system, sets up the GUI framework, and launches the main game loop 
- **TicTacToeGUI.cpp**: Complete GUI implementation with comprehensive event handling, user interaction management, and integration with core game components 

#### `/tests/`
The comprehensive testing suite ensures software quality and reliability through rigorous validation:

- **AIPlayer_test.cpp**: Unit tests specifically designed to validate AI component functionality, including minimax algorithm correctness and strategic decision-making capabilities 
- **GameBoard_test.cpp**: Thorough testing of game board logic, move validation, win condition detection, and grid state management 
- **GameHistory_test.cpp**: Validation of game history functionality, including secure storage, retrieval operations, and data integrity verification 
- **GameStateStack_test.cpp**: Testing of game state management, stack operations, and state transition reliability 
- **IntegrationTests.cpp**: System-wide integration tests that ensure all components work together seamlessly and maintain data consistency across modules 
- **main.cpp**: Test runner and Google Test framework setup that orchestrates the execution of all test suites and generates comprehensive coverage reports 
- **UserManager_test.cpp**: Authentication and user management testing, including password hashing validation, session security, and profile management functionality 

This modular architecture promotes maintainability, testability, and scalability while ensuring clear separation between game logic, user interface, and testing components.

## Usage

### Starting the Game
1. Launch the executable from the build directory
2. Create a new account or log in with existing credentials through the authentication system
3. Choose game mode (Player vs Player or Player vs AI)
4. Start playing on the interactive 3x3 grid

### Game Controls
- **Mouse Click**: Select grid position for your move through GUI interaction
- **Menu Navigation**: Use GUI buttons and forms for game options
- **History Access**: View and replay past games through the history interface

## Testing & Quality Assurance

### Testing Framework
- **Unit Tests**: Comprehensive component testing using Google Test framework
- **Integration Tests**: End-to-end system functionality validation ensuring all parts work together
- **Test Coverage**: Detailed reporting of code coverage metrics and test results

### Continuous Integration
- **GitHub Actions**: Automated testing and deployment pipeline for continuous integration
- **Code Quality**: Automated code style checks following Google C++ Style Guidelines
- **Performance Monitoring**: Response time, memory usage, and CPU utilization tracking

## Performance Optimization

The game includes comprehensive performance monitoring and optimization features:
- Response time measurement and optimization for efficient gameplay
- Memory usage tracking and management for resource efficiency
- CPU utilization monitoring for optimal system performance
- System resource efficiency improvements based on performance benchmarks

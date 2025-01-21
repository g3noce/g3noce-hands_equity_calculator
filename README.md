# Poker Hand Equity Calculator

A high-performance C program that calculates poker hand equities through Monte Carlo simulation. This tool helps poker players understand the relative strength of different starting hands by simulating millions of possible scenarios.

## Features

- Fast Monte Carlo simulation (10 million hands by default)
- Efficient hand evaluation algorithm using lookup tables
- Outputs results in a clear CSV matrix format
- Support for standard 52-card deck evaluations

## Technical Details

### Project Structure

- `main.c`: Program entry point and simulation logic
- `deck.c/h`: Deck initialization and shuffling functions
- `hand_evaluation.c/h`: Hand evaluation algorithms
- `lookup_tables.c/h`: Precomputed tables for fast hand evaluation

### Hand Evaluation

The program uses a sophisticated evaluation algorithm that:
- Evaluates 5-card poker hands using perfect hash functions
- Supports 7-card hand evaluation (2 hole cards + 5 community cards)
- Uses bitwise operations for optimal performance
- Implements the Two Plus Two evaluator algorithm

### Output Format

Results are saved in a CSV file with a matrix format where:
- Rows and columns represent card ranks (A-2)
- Cell values represent the average equity for each starting hand
- Paired hands appear on the diagonal
- Off-diagonal cells show suited (upper triangle) and offsuit (lower triangle) combinations

## Understanding the Results

The output CSV contains a 13x13 matrix where:
- Values represent winning probability (0.0 to 1.0)
- Higher values indicate stronger starting hands
- Example interpretations:
  - `1.0`: Always wins
  - `0.0`: Never wins

## Future Improvements

- Multi-threading support for faster simulation
- which combo (pair, three of a kind, full house, flush...) won the hand
- Custom simulation parameters via command line

## Acknowledgments

Hand evaluation algorithm inspired by the Two Plus Two Evaluator.

import chess
import chess.polyglot
import random

class ChessAI:
    def __init__(self, depth):
        self.depth = depth

    def minimax(self, board, depth, alpha, beta, maximizingPlayer):
        if depth == 0 or board.is_game_over():
            return self.evaluate(board)

        if maximizingPlayer:
            maxEval = float('-inf')
            for move in board.legal_moves:
                board.push(move)
                eval = self.minimax(board, depth - 1, alpha, beta, False)
                board.pop()
                maxEval = max(maxEval, eval)
                alpha = max(alpha, eval)
                if beta <= alpha:
                    break
            return maxEval
        else:
            minEval = float('inf')
            for move in board.legal_moves:
                board.push(move)
                eval = self.minimax(board, depth - 1, alpha, beta, True)
                board.pop()
                minEval = min(minEval, eval)
                beta = min(beta, eval)
                if beta <= alpha:
                    break
            return minEval

    def get_best_move(self, board):
        best_move = None
        maxEval = float('-inf')
        for move in board.legal_moves:
            board.push(move)
            eval = self.minimax(board, self.depth - 1, float('-inf'), float('inf'), False)
            board.pop()
            if eval > maxEval:
                maxEval = eval
                best_move = move
        return best_move

    def evaluate(self, board):
        # Simple evaluation function (can be improved)
        if board.is_checkmate():
            if board.turn:
                return float('-inf')  # AI lost
            else:
                return float('inf')  # AI won
        return random.random()  # Just a random evaluation for demonstration

# Create a chess board
board = chess.Board()

# Create AI instance, set search depth
ai = ChessAI(depth=3)

# Get the best move for the current board
best_move = ai.get_best_move(board)
print(f"The best move is: {best_move}")





class Solution(object):
    def solveSudoku(self, board):
        """
        :type board: List[List[str]]
        :rtype: None Do not return anything, modify board in-place instead.
        """
        def is_valid(n, i, j):
            # this number exists in the same column
            for l in range(0, 9):
                if board[l][j] == str(n):
                    return False
            # this number exists in the same row
            for l in range(0, 9):
                if board[i][l] == str(n):
                    return False
            # this number exists in the same box
            box_row = (i // 3) * 3
            box_col = (j // 3) * 3
            for r in range(box_row, box_row + 3):
                for c in range(box_col, box_col + 3):
                    if board[r][c] == str(n):
                        return False
            return True
        def helper(i, j):
            # if curr space is not empty: move to next space
            # if curr space is empty: input all numbers from 1-9 and solve sudoku given this new state
            # if i == 9 and j == 9: return board
            if i == 9 and j == 0:
                return True
            if j == 9:
                return helper(i + 1, 0)
            if board[i][j] != ".":
                return helper(i + 1 if j + 1 == 9 else i, 0 if j + 1 == 9 else j + 1)
            else:
                # curr space is empty
                for n in range(1, 10):
                    if is_valid(n, i, j):
                        board[i][j] = str(n)
                        if helper(i, j):
                            return True
                        board[i][j] = "."
            return False
        board = helper(0, 0)
        return board
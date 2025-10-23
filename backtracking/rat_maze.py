class Solution:
    def ratInMaze(self, maze):
        # code here
        N = len(maze)
        res = []
        def is_blocked(i, j):
            return True if maze[i][j] == 0 else False

        def is_visited(M, i, j):
            return True if M[i][j] == 1 else 0
        def helper(M, i, j, s):
            """
            M is a nxn matrix where m[i,j] is true if we visited it before
            i: row idx, j is the col idx
            """
            if [i, j] == [N - 1, N - 1]:
                res.append(s)
                return
            if i >= N or j >= N or is_blocked(i, j) or is_visited(M, i, j) \
                    or i < 0 or j < 0:
                return
                # mark as visited
            M[i][j] = 1

            # Explore up
            helper(M, i - 1, j, s + "U")
            # Explore right
            helper(M, i, j + 1, s + "R")
            # Explore left
            helper(M, i, j - 1, s + "L")
            # Explore down
            helper(M, i + 1, j, s + "D")
            M[i][j] = 0

        M = [] # visited matrix
        for i in range(N):
            l = [0] * N
            M.append(l)
        s = ""
        helper(M, 0, 0, s)
        res.sort()
        return res

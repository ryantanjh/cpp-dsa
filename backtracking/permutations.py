class Solution(object):
    def permute(self, nums):
        output = []
        def helper(arr, s):
            """
            arr: the currently filled array
            s: set of what has already been selected
            """
            if len(arr) == len(nums):
                c = arr[:]
                output.append(c)
                return
            for elem in nums:
                if elem not in s:
                    # not yet selected
                    arr.append(elem)
                    s.add(elem)
                    helper(arr, s)
                    # remove that elem
                    arr.remove(elem)
                    s.remove(elem)

        helper([], set())
        return output




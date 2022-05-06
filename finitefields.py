def inverses(p):
    result = [0, 1]
    for x in range(2, p):
        result.append(-(p // x * result[p % x]) % p)
    return result

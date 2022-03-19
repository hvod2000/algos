def graph_inverse(graph):
    G_inv = {}
    for u, vs in graph.items():
        for v in vs:
            G_inv.setdefault(v, set()).add(u)
    return G_inv


def contract_graph(graph, groups):
    v2g = {}
    for i, group in enumerate(groups):
        for u in group:
            v2g[u] = i
    for u in graph:
        if u not in v2g:
            v2g[u] = len(groups)
            groups.append({u})
    result = {u: set() for u in range(len(groups))}
    for i, group in enumerate(groups):
        result[i] = {v2g[v] for u in group for v in graph[u]} - {i}
    return result


if __name__ == "__main__":
    print("Non! Vous n'utilisez pas le module graphique correctement.")

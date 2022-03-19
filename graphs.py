def graph_inverse(graph):
    G_inv = {}
    for u, vs in graph.items():
        for v in vs:
            G_inv.setdefault(v, set()).add(u)
    return G_inv


if __name__ == "__main__":
    print("Non! Vous n'utilisez pas le module graphique correctement.")

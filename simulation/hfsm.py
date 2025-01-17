import graphviz

dot = graphviz.Digraph('CubeSat', comment='CubeSat State Machine', format='png')
dot.attr(compound='true', rankdir='TB')  # Global horizontal layout (Left to Right)

# Entry point
dot.node('Entry', '', shape='point')

# Initial state
dot.node('Launch1', 'Launch State', shape='box')
dot.edge('Entry', 'Launch1', label='Start')

# LEO superstate with vertical layout for Charge and Active side by side
with dot.subgraph(name='cluster_leo') as leo:
    leo.attr(label='LEO ', style='rounded', rankdir='LR')  # Vertical layout for Charge and Active
    
    leo.node('Leo1', 'entry:', shape='plaintext', style='dotted')
    leo.node('Leo2', 'exit:', shape='plaintext', style='dotted')

    # Charge as a superstate
    with leo.subgraph(name='cluster_charge') as active:
        active.attr(label='Charge', style='rounded', rankdir='TB')  # Active superstate layout
        active.node('Charge1', 'run: ', shape='plaintext')
        active.attr(rank='same')
    # Active as a superstate
    with leo.subgraph(name='cluster_active') as active:
        active.attr(label='Active', style='rounded', rankdir='TB')  # Active superstate layout
        active.node('Active1', 'run: ', shape='plaintext')
        active.attr(rank='same')
        
    leo.edge('Charge1', 'Active1', label='TICK_SIG \\\ Battery > 65%', ltail='cluster_charge', lhead='cluster_active')
    leo.edge('Active1', 'Charge1', label='TICK_SIG \\\ Battery < 60%', ltail='cluster_active', lhead='cluster_charge')

# Transitions from Launch to LEO
dot.edge('Launch1', 'Charge1', label='LEO Signal', lhead='cluster_leo')

# Render the graph
dot.render('cubesat_state_machine', view=True)

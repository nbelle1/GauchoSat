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



    leo.node('Leo1', 'CHARGE / {charge battery}', shape='plaintext', style='dotted')
    leo.node('Leo2', 'RADIO / {send message in buffer}', shape='plaintext', style='dotted')
    leo.node('Leo3', 'DE-ORBIT /', shape='plaintext', style='dotted')


    with leo.subgraph(name='cluster_deployment') as deployment:
        deployment.attr(label='Deployment', style='rounded', rankdir='TB')  # Active superstate layout
        deployment.node('Deployment1', 'TICK / (CHARGE -> SYSTEMS_CHECK)', shape='plaintext')
        deployment.node('Deployment2', 'SYSTEMS_CHECK / (Test System -> RADIO["systems nominal"])', shape='plaintext')
        deployment.attr(rank='same')

    with leo.subgraph(name='cluster_detumble') as detumble:
        detumble.attr(label='Detumble', style='rounded', rankdir='TB')  # Active superstate layout
        detumble.node('Detumble1', 'TICK / (CHARGE -> TELEMETRY -> RADIO[send rotational velocity] -> Detumble)', shape='plaintext')
        detumble.node('Detumble2', 'DETUMBLE / (MagneTorquer Adjustment According to Rotational Velocity)', shape='plaintext')
        detumble.node('Detumble3', 'TELEMETRY / (Check rotational velocity)', shape='plaintext')
        detumble.attr(rank='same')

    with leo.subgraph(name='cluster_telemetry') as telemetry:
        telemetry.attr(label='Telemetry', style='rounded', rankdir='TB')  # Active superstate layout
        telemetry.node('Telemetry1', 'TICK / (CHARGE -> TELEMETRY -> RADIO[send data])', shape='plaintext')
        telemetry.node('Telemetry2', 'TELEMETRY / (Collect & Store Data)', shape='plaintext')

        telemetry.attr(rank='same')
        
    leo.edge('Deployment1', 'Detumble1', label='SYSTEMS_CHECK / System Check Done ', ltail='cluster_deployment', lhead='cluster_detumble')
    leo.edge('Detumble1', 'Telemetry1', label='TICK / Detumble Done & RADIO["Detumble Done"]', ltail='cluster_detumble', lhead='cluster_telemetry')


# Transitions from Launch to LEO
dot.edge('Launch1', 'Deployment1', label='LEO Signal', lhead='cluster_deployment')

# Render the graph
dot.render('cubesat_state_machine', view=True)

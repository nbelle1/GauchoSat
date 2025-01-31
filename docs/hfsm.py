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
    leo.attr(label='LEO', style='rounded', rankdir='LR')  # Horizontal layout for the main cluster

    # Nodes for CHARGE, RADIO, and DE-ORBIT
    leo.node('Leo1', 'TICK / {checkBatterylvl()}', shape='plaintext', style='dotted')

    # Subgraph for "Idle Charging Batteries"
    with leo.subgraph(name='cluster_charge') as charge:
        charge.attr(label='Idle Charging Batteries', style='rounded', rankdir='TB')  # Top to Bottom
        charge.node('Charge1', 'Entry / {TURN OFF ALL COMPONENTS}', shape='plaintext')

    # Subgraph for "Active"
    with leo.subgraph(name='cluster_active') as active:
        active.attr(label='Active', style='rounded', rankdir='TB')  # Top to Bottom
        active.node('Active1', '', shape='point')

        # Subgraph for "Payload"
        with active.subgraph(name='cluster_payload') as payload:
            payload.attr(label='Payload', style='rounded', rankdir='TB')  # Top to Bottom
            payload.node('Payload1', '', shape='plaintext')

            # Subgraph for "Detumble"
            with payload.subgraph(name='cluster_detumble') as detumble:
                detumble.attr(label='Detumble', style='rounded', rankdir='TB')  # Top to Bottom
                detumble.node('Detumble1', 'Entry / {TURN ON ADCS - RUN ADCS & ORIENT}', shape='plaintext')
                detumble.node('Detumble2', 'EXIT / {TURN OFF ADCS}', shape='plaintext')

            # Subgraph for "Telemetry"
            with payload.subgraph(name='cluster_telemetry') as telemetry:
                telemetry.attr(label='Telemetry', style='rounded', rankdir='TB')  # Top to Bottom
                telemetry.node('Telemetry1', 'Entry / {GET DATA FROM SENSORS - SAVE DATA IN BUFFER - SET R_TO_TRANSMIT}', shape='plaintext')
                telemetry.node('Telemetry2', 'EXIT / {TURN OFF TELEMETRY}', shape='plaintext')
            
            payload.edge('Detumble1', 'Telemetry1', label='', ltail='cluster_detumble', lhead='cluster_telemetry')

        # Subgraph for "Radio"
        with active.subgraph(name='cluster_radio') as radio:
            radio.attr(label='Radio', style='rounded', rankdir='TB')  # Top to Bottom
            radio.node('Radio1', 'ENTRY / {TURN ON RADIO}', shape='plaintext')
            radio.node('Radio2', 'EXIT / {TURN OFF RADIO}', shape='plaintext')

            # Subgraph for "Transmit Data"
            with radio.subgraph(name='cluster_transmit') as transmit:
                transmit.attr(label='Transmit Data', style='rounded', rankdir='TB')  # Top to Bottom
                transmit.node('Transmit1', 'Entry / ()', shape='plaintext')

            # Subgraph for "Receive Data"
            with radio.subgraph(name='cluster_receive') as receive:
                receive.attr(label='Receive Data', style='rounded', rankdir='TB')  # Top to Bottom
                receive.node('Receive1', 'Entry / ()', shape='plaintext')

            # radio.edge('Radio1', 'Transmit1', label='', lhead='cluster_transmit')
            radio.edge('Transmit1', 'Receive1', label='TX_TIMEOUT(2 MIN)', ltail='cluster_transmit', lhead='cluster_receive')
            # radio.edge('Receive1', 'Transmit1', label='TX_REQUESTED', ltail='cluster_receive', lhead='cluster_transmit')

        active.edge('Active1', 'Detumble1', label='Start', lhead='cluster_detumble')
        active.edge('Payload1', 'Transmit1', label='R_TO_TRANSMIT', ltail='cluster_payload', lhead='cluster_transmit')
        active.edge('Receive1', 'Payload1', label='RX_TIMEOUT(2 MIN)', ltail='cluster_receive', lhead='cluster_payload')

    # leo.edge('Active1', 'Charge1', label='FAILSAFE: BATTERIES < 20%', ltail='cluster_active', lhead='cluster_charge')
    leo.edge('Charge1', 'Active1', label='BATTERIES > 80%', ltail='cluster_charge', lhead='cluster_active')
    leo.edge('Active1', 'Charge1', label='BATTERIES < 30%' , ltail='cluster_active', lhead='cluster_charge')
    



    

# Transitions from Launch to LEO
dot.edge('Launch1', 'Charge1', label='LEO Signal', lhead='cluster_charge')

# Render the graph
dot.render('cubesat_state_machine', view=True)

#!/usr/bin/env python3

# A small example of generating a dot file to visulalize the link graph

import re
import os
import sys
import yaml

template = """digraph D {

node [fontname="Arial"];

%s
}"""

def main(yamlfile):

    with open(yamlfile, "r") as stream:
        try:
            content = yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            sys.exit(exc)

    events = content.get('auditlib', {}).get("audits", {})
    if not events:
        sys.exit("Improperly formatted file, cannot find auditlib -> audits!")

    nodes = {}
    for event in events:
        if event['event'] == "object_loaded":
            nodes[event['identifier']] = event   
            basename = os.path.basename(event['name']) or "root"
            nodes[event['identifier']]['slug'] = re.sub("(-|_|[.])", '_', basename)

    # Define the nodes
    output = ""
    for nid, node in nodes.items():
        output += "%s [shape=circle]\n" % (node['slug'])

    # For each event, see if the searched for library is then found
    for e, event in enumerate(events):
        if event['event'] == "searching_for":

            # The next event is usually the find?
            next_event = None
            i = e + 1
            while True:            
                next_event = events[i]
                if next_event['event'] != "object_loaded":
                    i+=1
                    if i > len(events) -1:
                       next_event = None
                       break
                else:
                    break       

            # This is a child library of the event in question
            node = nodes.get(event['initiated_by'])
            if next_event and node:
                name1 = nodes[event['initiated_by']]['slug']
                name2 = nodes[next_event['identifier']]['slug']
                output += "%s -> %s\n" % (name1, name2)
                
    print(template % output)          

if __name__ == "__main__":
    if len(sys.argv) == 1:
        sys.exit("Please provide a yaml output file to parse!")
    main(sys.argv[1])


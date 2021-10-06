#!/usr/bin/env python3

# A small example of generating a dot file to visulalize the link graph

import random
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

    events = content.get("auditlib", {}).get("audits", {})
    if not events:
        sys.exit("Improperly formatted file, cannot find auditlib -> audits!")

    # We will assign colors based on the parent (or white for root)
    colors = [
        "#" + "".join([random.choice("0123456789ABCDEF") for j in range(6)])
        for i in range(len(events))
    ]

    nodes = {}
    for event in events:
        if event["event"] == "object_loaded":
            nodes[event["identifier"]] = event
            basename = os.path.basename(event["name"]) or "root"
            nodes[event["identifier"]]["slug"] = re.sub("(-|_|[.])", "_", basename)
            nodes[event["identifier"]]["color"] = colors.pop()

    # Keep track of relations we've seen
    seen = set()

    # Keep track of child / parents
    parents = {}

    # Keep output string
    relations = ""

    # For each event, see if the searched for library is then found
    for e, event in enumerate(events):
        if event["event"] == "searching_for":

            # The next event is usually the find?
            next_event = None
            i = e + 1
            while True:
                next_event = events[i]
                if next_event["event"] != "object_loaded":
                    i += 1
                    if i > len(events) - 1:
                        next_event = None
                        break
                else:
                    break

            # This is a child library of the event in question
            node = nodes.get(event["initiated_by"])
            if next_event and node:

                # Keep a lookup for the parent node for coloring
                id1 = nodes[event["initiated_by"]]["identifier"]
                id2 = nodes[next_event["identifier"]]["identifier"]
                parents[id2] = id1

                name1 = nodes[event["initiated_by"]]["slug"]
                name2 = nodes[next_event["identifier"]]["slug"]
                if "%s->%s" % (name1, name2) in seen:
                    continue
                relations += "%s -> %s\n" % (name1, name2)
                seen.add("%s->%s" % (name1, name2))

    # Define the nodes
    output = ""
    for nid, node in nodes.items():
        shape = "box"
        if node["slug"] == "root":
            shape = "circle"

        # Figure out the parent for the color
        parent = parents.get(node["identifier"])
        color = ""
        if parent:
            color = ',style=filled,color="%s"' % nodes[parent]["color"]
        output += "%s [shape=%s%s]\n" % (node["slug"], shape, color)

    output = output + "\n" + relations
    print(template % output)


if __name__ == "__main__":
    if len(sys.argv) == 1:
        sys.exit("Please provide a yaml output file to parse!")
    main(sys.argv[1])

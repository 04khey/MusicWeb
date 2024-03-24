# MusicWeb
CLI Music playlist generator via traversal of 2D graph representation of music library. Spotify killer.

# Motivation
Playlists don't cut it. Some songs fit into multiple playlists, or somewhere in-between two or more playlists.<br>
Some songs tonally clash and shouldn't be played one-after-another, but this means shuffling a large playlist with these songs in is risky.<br>
Then again, playing without shuffling can become stale, and using several small playlists realistically means you will never listen to some.<br>
Sometimes you aren't sure what you're in the mood to listen to other than the general genre of music.<br>
<br>
Solution: songs are represented as nodes on an (un)directed graph, with weights from the current node representing relative likelihood of enqueueing. <br>
Therefore all successive songs will be tonally similar, or otherwise acceptable to follow based on the user's designs.<br>
A 'trail' can be left behind to discourage recently listened nodes from being played again. <br>
A 'tonal super skip' can be implemented by jumping along many nodes, or to a tagged genre node. <br>
This structure allows for particularly complex playlist/DJ structures to be created, and contains normal playlist files as a subset.



# Status
Was paused since I realised the CLI needed to be rewritten using NCurses.

# Dependencies:

cmus<br>
cmus-remote<br>
screen

# Feature checklist
* Lazy track selection (buffer) via cmus feedback                   ☐
* read from STDIN                                                   ☐
* playlist viewer (CLI)                                             ☐
* Add node to existing playlist                                     ☐
* Unicode support?                                                  ✓
* Interface with cmus:                                          
* Detect nonexistent songs                                          ☐
    - Support for skipping                                          ☐
    - Hashmap on song names for quick lookup for playlist merging   ☐
* Better practice with types                                        ☐
* Windows support via cygwin bundling                               ☐

# TODO
rewrite using ncurses

# Others' Suggestions
* Spotify integration for wrapped boasting
* Capacity to stream songs (would need some library indexing method)
* GUI

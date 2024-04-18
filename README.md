# MusicWeb
CLI Music playlist generator via traversal of N-Dimensional graph representation of music library. Spotify killer.

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
**[In development]**<br>

# Dependencies:
## Build: 
libncurses-dev<br>
## Run:
cmus<br>
screen

# Feature checklist
* Basic functionality                                               ☐
* Lazy track selection (buffer) via cmus feedback                   ☐
* playlist visualiser (open graphical window)                       ☐
* Unicode support?                                                  ~ (accidental)
* Real Unicode IME support                                          ☐
* Better practice with types                                        ☐
* Config file for keybinds                                          ☐
* Track seaching and metadata pulling                               ☐
* Windows support via cygwin bundling                               ☐

# TODO
rewrite using ncurses

# Others' Suggestions
* Spotify integration for wrapped boasting (use spotifyd instead of cmus, would require creating LIBRARY of urls rather than URIs (scan all songs in a massive playlist?), seems doable...)
* Use Last FM Scrobbling API for smae reasons as above 
* Capacity to stream songs (would need some library indexing method)
* GUI
* Unethical spotify to mp3 script for convenient setup?

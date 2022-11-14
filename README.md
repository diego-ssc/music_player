# Setup

Run, in the root directory:
```plaintext
meson build
```


Then, to compile the program:
```plaintext
meson compile -C build/
```

# Usage
Run, in the root directory:
```plaintext
./build/music_player
```

A window will pop up.
Three buttons will be available to trigger.

At the far left corner you will be able to add
directories that contain .mp3 files into the database
located in

```plaintext
home/[your_username]/.local/share/music_player/music.db
```

To the right, a button with a lens icon will be located.
Press it to trigger a search entry that will autocomplete
the song names (already present in the database) that you
enter.

It can also accept filters such as genre, track, year and path
with the following syntax.

```plaintext
[song_name] [flag]::[value]
```

For example, if you want to search for the song with name 'Main Theme',
that has the 'Game' genre, you would do it like this:

```plaintext
Main Theme genre::Game
```

When you press enter, the song should be appear and be available to be
played with the media controls located at the bottom of the window.

At the top right corner a toggle button will be located, when pressed,
it will show the songs that are currently loaded into the database.

Clicking over a row (a song) will allocate it as a playable song,
the cover will appear at the center of the window and the media controls
will be available to use.

Lastly, the media controls will be located at the bottom of the window.
They will in charge of playing, pausing and seeking into a specific
portion of the song. You can also adjust the volume with it.

The in_group, person/group definition, album queries and data
modification functionalities were not implemented.

The queries don't work as I had planned; it's impossible to make a query
that involves more than one extra flag (considering, also, the lack of
flags like album).

# Design

The model-view-controller pattern was used and mainly done by the
setup the gtk library suggests.

The controller is represented by the Application/Application_Window class,
the view is determined by the .ui files located in the data directory.

And the model is given by the Miner, Decoder, Model_Columns and
Database_Queries classes.

The miner has the role of searchen in a given directory for .mp3 files. It
also has the duty of opening the database, creating it, if it does not exist,
and adding the found songs into the database (verifying, previously, if
they are not already contained in it).

The Model_Columns class is the structure (of the model, actually) of the treeview
that holds the songs that are contained in the database.

The Decoder handles the information located in the tags of the .mp3 files;
including the title, album, cover, track, year, etc.
It will provide the tools to extract the tag of the .mp3 file and the cover of
the songs.

The Database_Queries object was meant to be the object that executes sends and receives
the queries to the database. Or at least the ones that involve the search entry widget and
the consistency of the database.
(The Miner class does queries to initialize the database, for example).

# Dependencies

```plaintext
gstreamer
https://archlinux.org/packages/?name=gstreamer
```

```plaintext
gst-libav
https://archlinux.org/packages/?name=gst-libav
```

```plaintext
gst-plugins-base
https://archlinux.org/packages/?name=gst-plugins-base
```

```plaintext
gst-plugins-good
https://archlinux.org/packages/?name=gst-plugins-good
```

```plaintext
gtkmm-4.0
https://archlinux.org/packages/extra/x86_64/gtkmm-4.0/
```
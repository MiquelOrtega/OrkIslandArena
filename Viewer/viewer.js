
// *********************************************************************
// Global variables
// *********************************************************************

// Viewer state.
var gamePaused = true;
var gamePreview = false; // If true, render will be called for the next tick even if game is paused, and then will be set to false.
var gameAnim = true;
var gameDirection = 1;
var actRound = 0; // Current round index.


// Data.
var raw_data_str; // String for storing the raw data.
var dataLoaded = false; // Set to true when raw_data_str is ready to be parsed.
var data = { } // Object for storing all the game data.


// Animation.
var speed = 10; // Ticks per second.
var FRAMES_PER_ROUND = 2;
var frames = 0; // Incremented each tick, when it reaches FRAMES_PER_ROUND, actRound is updated (acording to gameDirection).


// Visuals.
var unitSize = 0.85; // 1 = same size as tile.
var unitLineWidth = 2;
var grid_color = "#000000"; // Black.


var cell_colors = {
    'W': "#00FFFF", // Light blue.
    '.': "#AAFFAA", // Light green.
    'F': "#6CA972", // Deep green.
    'S': "#FFF0B3", // Light yellow.
    'C': "#404040", // Deep grey.
    'P': "#999999"  // Light grey.
}


var player_colors = {
    0: "#FF0000",  // Red.
    1: "#FFCC00",  // Golden.
    2: "#0080FF",  // Deep blue.
    3: "#FFCCFF"   // Pink.
}


// *********************************************************************
// Utility functions
// *********************************************************************

function getURLParameter (name) {
    // http://stackoverflow.com/questions/1403888/get-url-parameter-with-jquery
    var a = (RegExp(name + '=' + '(.+?)(&|$)').exec(location.search)||[,null])[1]
    if (a != null) return decodeURI(a);
    return null;
}


// Callback has a single parameter with the file contents.
function loadFile (file, callback) {
    var xmlhttp;

    if (file == null || file == "") {
        alert("You must specify a file to load.");
        return;
    }

    if (window.XMLHttpRequest) xmlhttp = new XMLHttpRequest(); // Code for IE7+, Firefox, Chrome, Opera, Safari.
    else xmlhttp = new ActiveXObject("Microsoft.XMLHTTP"); // Code for IE6, IE5.

    // http://www.w3schools.com/ajax/ajax_xmlhttprequest_onreadystatechange.asp
    xmlhttp.onreadystatechange = function() {
        // Note: We cannot check xmlhttp.status != 200 for errors because status is not set when loading local files.
        if (xmlhttp.readyState == 4) callback(xmlhttp.responseText);
    }

    xmlhttp.open("GET", file, false);
    xmlhttp.send();
}


function int (s) {
    return parseInt(s);
}


function double (s) {
    return parseFloat(s);
}


function parse_assert (read_value, expected_value) {
    var correct = (read_value == expected_value);
    if (!correct) alert("Error parsing file, expected token: " + expected_value + ", read token: " + read_value + ".");
    return correct;
}


// *********************************************************************
// Initialization functions
// *********************************************************************

function parseData (raw_data_str) {
    if (raw_data_str == "") {
        alert("Could not load game file.");
        return false;
    }

    // Convert text to tokens.
    var st = raw_data_str + "";
    var t = st.replace('\n', ' ').split(/\s+/);
    var p = 0;

    data.secgame = (t[p++] == "SecGame");

    parse_assert(t[p++], "Seed");
    data.seed = int(t[p++]);

    // Game and version.
    if (t[p++] != "OrkIsland") {
        alert("Are you sure this is an OrkIsland game file?");
        document.getElementById('file').value = "";
        document.getElementById('inputdiv').style.display = "";
        document.getElementById('loadingdiv').style.display = "none";
        return false;
    }

    data.version = t[p++];
    if (data.version != "1.0") alert("Unsupported game version! Trying to load it anyway.");

    parse_assert(t[p++], "nb_players");
    data.nb_players = int(t[p++]);

    parse_assert(t[p++], "rows");
    data.rows = int(t[p++]);

    parse_assert(t[p++], "cols");
    data.cols = int(t[p++]);

    parse_assert(t[p++], "nb_rounds");
    data.nb_rounds = int(t[p++]);

    parse_assert(t[p++], "initial_health");
    data.initial_health = int(t[p++]);

    parse_assert(t[p++], "nb_orks");
    data.nb_orks = int(t[p++]);

    parse_assert(t[p++], "cost_grass");
    data.cost_grass = int(t[p++]);

    parse_assert(t[p++], "cost_forest");
    data.cost_forest = int(t[p++]);

    parse_assert(t[p++], "cost_sand");
    data.cost_sand = int(t[p++]);

    parse_assert(t[p++], "cost_city");
    data.cost_city = int(t[p++]);

    parse_assert(t[p++], "cost_path");
    data.cost_path = int(t[p++]);

    parse_assert(t[p++], "bonus_per_city_cell");
    data.bonus_per_city_cell = int(t[p++]);

    parse_assert(t[p++], "bonus_per_path_cell");
    data.bonus_per_path_cell = int(t[p++]);

    parse_assert(t[p++], "factor_connected_component");
    data.factor_connected_component = int(t[p++]);

    data.nb_units = data.nb_players * data.nb_orks;

    parse_assert(t[p++], "names");
    data.names = new Array();
    for (var i = 0; i < data.nb_players; ++i) data.names[i] = t[p++];

    data.rounds = new Array();
    for (var round = 0; round <= data.nb_rounds; ++round) {

        // Maze.
        t[p++]; // 1st row of column labels.
        t[p++]; // 2nd row of column labels.

        data.rounds[round] = new Object();
        data.rounds[round].rows = new Array();
        for (var i = 0; i < data.rows; ++i) {
            parse_assert(t[p++], i);
            data.rounds[round].rows[i] = t[p++];
        }

        // Cities.
        parse_assert(t[p++], "cities");
        data.rounds[round].nb_cities = int(t[p++]);
        data.rounds[round].cities = new Array();
        for (var i = 0; i < data.rounds[round].nb_cities; ++i) {
            data.rounds[round].cities[i] = new Object();
            data.rounds[round].cities[i].size = int(t[p++]);
            data.rounds[round].cities[i].cell = new Array();
            for (var j = 0; j < data.rounds[round].cities[i].size; ++j) {
                data.rounds[round].cities[i].cell[j] = new Object();
                data.rounds[round].cities[i].cell[j].i = int(t[p++]);
                data.rounds[round].cities[i].cell[j].j = int(t[p++]);
            }
        }

        // Paths.
        parse_assert(t[p++], "paths");
        data.rounds[round].nb_paths = int(t[p++]);
        data.rounds[round].paths = new Array();
        for (var i = 0; i < data.rounds[round].nb_paths; ++i) {
            data.rounds[round].paths[i] = new Object();
            data.rounds[round].paths[i].a    = int(t[p++]);
            data.rounds[round].paths[i].b    = int(t[p++]);
            data.rounds[round].paths[i].size = int(t[p++]);
            data.rounds[round].paths[i].cell = new Array();
            for (var j = 0; j < data.rounds[round].paths[i].size; ++j) {
                data.rounds[round].paths[i].cell[j] = new Object();
                data.rounds[round].paths[i].cell[j].i = int(t[p++]);
                data.rounds[round].paths[i].cell[j].j = int(t[p++]);
            }
        }

        parse_assert(t[p++], "round");
        if (int(t[p++]) != round) alert("Wrong round number!");

        // Total score.
        parse_assert(t[p++], "total_score");
        data.rounds[round].total_score = new Array();
        for (var i = 0; i < data.nb_players; ++i) data.rounds[round].total_score[i] = int(t[p++]);

        // Status.
        parse_assert(t[p++], "status");
        data.rounds[round].cpu = new Array();
        for (var i = 0; i < data.nb_players; ++i) {
            var cpu = int(double(t[p++])*100);
            data.rounds[round].cpu[i] = (cpu == -100) ? "out" : cpu + "%";
        }

        // City owners.
        parse_assert(t[p++], "city_owners");
        data.rounds[round].city_owners = new Array();
        for (var i = 0; i < data.rounds[round].nb_cities; ++i) {
            data.rounds[round].city_owners[i] = int(t[p++]);
        }

        // Path owners.
        parse_assert(t[p++], "path_owners");
        data.rounds[round].path_owners = new Array();
        for (var i = 0; i < data.rounds[round].nb_paths; ++i) {
            data.rounds[round].path_owners[i] = int(t[p++]);
        }

        // Units.
        parse_assert(t[p++], "units");
        data.rounds[round].units = new Array();
        for (var i = 0; i < data.nb_units; ++i) {
            data.rounds[round].units[i] = new Object();
            data.rounds[round].units[i].player = int(t[p++]);
            data.rounds[round].units[i].i      = int(t[p++]);
            data.rounds[round].units[i].j      = int(t[p++]);
            data.rounds[round].units[i].health = int(t[p++]);
        }

        if (round != data.nb_rounds) {
            // Movements.
            parse_assert(t[p++], "commands");
            var code = int(t[p++]);
            while (code != -1) {
                data.rounds[round].units[code].move = t[p++];
                code = int(t[p++]);
            }
        }
    }
    return true;
}


// Initializing the game.
function initGame (raw_data) {
    document.getElementById("loadingdiv").style.display = "";

    // TODO: Next two calls could run concurrently.
    if (parseData(raw_data) === false) return;
    preloadImages();

    // Prepare state variables.
    // if (getURLParameter("start") == "yes") gamePaused = false;
    // else gamePaused = true;
    gamePaused = false;

    gamePreview = true;

    // Canvas element.
    canvas = document.getElementById('myCanvas');
    context = canvas.getContext("2d");

    // Prepare the slider.
    $("#slider").slider({
        slide: function(event, ui) {
            var value = $("#slider").slider( "option", "value" );
            actRound = value;
            frames = 0;
            gamePaused = true;
            gamePreview = true;
        }
    });
    $("#slider").width(600);
    $("#slider").slider("option", {min: 0, max: data.nb_rounds});

    // Set the listerners for interaction.
    document.addEventListener('mousewheel', onDocumentMouseWheel, false);
    document.addEventListener('keydown', onDocumentKeyDown, false);
    document.addEventListener('keyup', onDocumentKeyUp, false);

    window.addEventListener('resize', onWindowResize, false);
    onWindowResize();

    document.getElementById("loadingdiv").style.display = "none";
    document.getElementById("gamediv").style.display = "";

    mainloop();
}


function preloadImages () {
    data.img = new Array();

    // Background.
    // data.img.background = new Image();
    // data.img.background.src = "img/tile_floor.png";
    data.img.tile = new Image();
    data.img.tile.src = "img/tile.png";
}


// *********************************************************************
// Main loop functions
// *********************************************************************

function updateGame () {
    /*
      if (actRound >= 0 && actRound < data.nb_rounds) {
      for (var i = 0; i < data.nb_players; ++i) {
      var f = (frames+i)%4;
      switch (data.rounds[actRound].team[i].pacman.action) {
      case 't': //Top
      data.img.spr_pacman[i] = data.img.spr_pacman_t[i][f];
      break;
      case 'b': //Bottom
      data.img.spr_pacman[i] = data.img.spr_pacman_b[i][f];
      break;
      case 'r': //Right
      data.img.spr_pacman[i] = data.img.spr_pacman_r[i][f];
      break;
      case 'l': //Left
      data.img.spr_pacman[i] = data.img.spr_pacman_l[i][f];
      break;
      default: //None
      data.img.spr_pacman[i] = data.img.spr_pacman_r[i][f];
      break;
      }
      }
      }
    */
    $("#slider").slider("option", "value", actRound);
}


function writeGameState () {
    // Write round.
    $("#round").html("Round: " + actRound);

    // Update scoreboard.
    var scoreboard = "";
    for (var i = 0; i < data.nb_players; ++i) {
        scoreboard += "<span class='score'>"
            + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" + player_colors[i] + "'></div>"
            + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>" + data.names[i] + "</div>"
            + "<br/>"
            + "<div style='margin-left: 10px;'>"
            // + "<div style='padding:2px;'>Land: " + data.rounds[actRound].land[i] + "</div>"
            + (data.secgame ? "<div style='padding:2px;'>CPU: " + data.rounds[actRound].cpu[i] + "</div>" : "")
            + "</div>"
            + "</span><br/><br/>";
    }
    $("#scores").html(scoreboard);

    var order = [0, 1, 2, 3];
    for (var i = 0; i < 3; ++i) {
        for (var j = i + 1; j < 4; ++j) {
            if (data.rounds[actRound].total_score[order[i]] < data.rounds[actRound].total_score[order[j]]) {
                var k = order[i];
                order[i] = order[j];
                order[j] = k;
            }
        }
    }

    var totalboard = "";
    for (var i = 0; i < data.nb_players; ++i) {
        totalboard += "<span class='total'>"
            + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" + player_colors[order[i]] + "'></div>"
            + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>"
            + data.rounds[actRound].total_score[order[i]] + "</div>"
            + "</span><br/><br/>";
    }
    $("#totals").html(totalboard);
}


function drawGame () {
    // Boundary check.
    if (actRound < 0) actRound = 0;
    if (actRound >= data.nb_rounds) actRound = data.nb_rounds;

    // Outter Rectangle.
    // Uncommented so as not to draw the grid.
    // context.fillStyle = grid_color;
    // context.fillRect(0, 0, tileSize*data.cols, tileSize*data.rows);

    // Draw maze.
    var rows = data.rounds[actRound].rows;
    for (var i = 0; i < data.rows; ++i) {
        var row = rows[i];
        for (var j = 0; j < data.cols; ++j) {
            var cell = row[j];
            context.fillStyle = cell_colors[cell];
            context.fillRect(j*tileSize, i*tileSize, tileSize, tileSize); // -1 to show a grid.
        }
    }

    // Draw cities.
    for (var i = 0; i < data.rounds[actRound].nb_cities; ++i) {
        var city  = data.rounds[actRound].cities[i];
        var owner = data.rounds[actRound].city_owners[i];
        if (owner != -1) {
            context.strokeStyle = player_colors[owner];
            context.fillStyle   = player_colors[owner];
            for (var j = 0; j < city.size; ++j) {
                drawCross(city.cell[j].i, city.cell[j].j);
            }
        }
    }

    // Draw paths.
    for (var i = 0; i < data.rounds[actRound].nb_paths; ++i) {
        var path  = data.rounds[actRound].paths[i];
        var owner = data.rounds[actRound].path_owners[i];
        if (owner != -1) {
            context.strokeStyle = player_colors[owner];
            context.fillStyle   = player_colors[owner];
            for (var j = 0; j < path.size; ++j) {
                drawCross(path.cell[j].i, path.cell[j].j);
            }
        }
    }
    
    // Draw units.
    context.lineWidth = unitLineWidth;
    var units = data.rounds[actRound].units;
    for (var un in units) {
        var u = units[un];
        context.strokeStyle = player_colors[u.player];
        context.fillStyle = player_colors[u.player];
        var i = u.i;
        var j = u.j;

        if (gameAnim) {
            if (frames >= FRAMES_PER_ROUND/2) {
                if (u.move == 'b') i += 0.5;
                else if (u.move == 'r') j += 0.5;
                else if (u.move == 't') i -= 0.5;
                else if (u.move == 'l') j -= 0.5;
            }
        }
        drawSquare(i, j);
    }
}


function drawSquare (i, j) {
    var size = unitSize * tileSize;
    var offset = (tileSize - size) / 2;
    context.fillRect(j*tileSize + offset, i*tileSize + offset, size, size);
}

function drawCircle(i, j) {
    var size = unitSize * tileSize * 0.4;
    var offset = (tileSize - size) / 2;
    context.beginPath();
    context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/3, 0, Math.PI*2, false);
    context.fill();
    context.stroke();
}

function drawCross (i, j) {
  var size = unitSize * tileSize * 0.6;
  var offset = (tileSize - size) / 2;
  // context.beginPath();
  // context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/2, 0, Math.PI*2, false);
  // context.fill();
  // context.stroke();
  context.lineWidth=0.75;
  context.beginPath();
  context.moveTo(j*tileSize + offset - 0.4*size, i*tileSize + offset - 0.4*size);
  context.lineTo(j*tileSize + offset + 1.4*size, i*tileSize + offset + 1.4*size);
  context.stroke();
  context.beginPath();
  context.moveTo(j*tileSize + offset + 1.4*size, i*tileSize + offset - 0.4*size);
  context.lineTo(j*tileSize + offset - 0.4*size, i*tileSize + offset + 1.4*size);
  context.stroke();
}

// *********************************************************************
// Button events
// *********************************************************************

function playButton () {
    gamePaused = false;
}


function pauseButton () {
    gamePaused = true;
    gamePreview = true; // To call render again.
    frames = 0;
}


function startButton () {
    gamePaused = true;
    gamePreview = true;
    frames = 0;
    actRound = 0;
}


function endButton () {
    gamePreview = true;
    frames = 0;
    actRound = data.nb_rounds;
}


function animButton () {
    gameAnim = !gameAnim;
}


function closeButton () {
    window.close();
}


function helpButton () {
    help();
}

// *********************************************************************
// Keyboard and Mouse events
// *********************************************************************

function onDocumentMouseWheel (event) {
}


function onDocumentKeyDown (event) {
}


function onDocumentKeyUp (event) {
    // http://www.webonweboff.com/tips/js/event_key_codes.aspx
    switch (event.keyCode) {
    case 36: // Start.
        gamePreview = true;
        actRound = 0;
        frames = 0;
        break;

    case 35: // End.
        gamePreview = true;
        actRound = data.nb_rounds;
        frames = 0;
        break;

    case 33: // PageDown.
        gamePreview = true;
        actRound -= 10;
        frames = 0;
        break;

    case 34: // PageUp.
        gamePreview = true;
        actRound += 10;
        frames = 0;
        break;

    case 38: // ArrowUp.
    case 37: // ArrowLeft.
        gamePaused= true;
        gamePreview = true;
        --actRound;
        frames = 0;
        break;

    case 40: // ArrowDown.
    case 39: // ArrowRight.
        gamePaused = true;
        gamePreview = true;
        ++actRound;
        frames = 0;
        break;

    case 32: // Space.
        if (gamePaused) playButton();
        else pauseButton();
        break;

    case 72: // "h"
        helpButton();
        break;

    default:
        // $("#debug").html(event.keyCode);
        break;
    }
}


function onWindowResize (event) {
    // Constants.
    var header_height = 150;
    var canvas_margin = 0; // 20;

    // Set canvas size.
    var size = Math.min(document.body.offsetWidth, document.body.offsetHeight - header_height) - canvas_margin*2;

    canvas.width  = size;
    canvas.height = size;

    var max_dimension = Math.max(data.cols,data.rows);
    tileSize = size / max_dimension;

    drawGame();
}


function help () {
    // Opens a new popup with the help page.
    var win = window.open('help.html', 'name', 'height=400, width=300');
    if (window.focus) win.focus();
    return false;
}


// *********************************************************************
// This function is called periodically.
// *********************************************************************

function mainloop () {
    // Configure buttons.
    if (gamePaused) {
        $("#but_play").show();
        $("#but_pause").hide();
    }
    else {
        $("#but_play").hide();
        $("#but_pause").show();
    }

    if (actRound < 0) actRound = 0;

    if (actRound > data.nb_rounds) {
        actRound = data.nb_rounds;
        gamePaused = true;
        frames = 0;
    }

    if (!gamePaused || gamePreview) {
        updateGame();
        drawGame();
        writeGameState();

        if (gamePreview) {
            frames = 0;
            gamePreview = false;
        }
        else {
            ++frames;
            if (frames == FRAMES_PER_ROUND) {
                frames = 0;
                actRound += gameDirection;
            }
        }
    }

    // Periodically call mainloop.
    var frame_time = 1000/speed;
    setTimeout(mainloop, frame_time);
}


// *********************************************************************
// Main function, it is called when the document is ready.
// *********************************************************************

function init () {
    // Get url parameters.
    var game;
    if (getURLParameter("sub") != null) {
        var domain = window.location.protocol + "//" + window.location.host;
        if (getURLParameter("nbr") != null)
            game = domain + "/?cmd=lliuraments&sub=" + getURLParameter("sub") + "&nbr=" + getURLParameter("nbr") + "&download=partida";
        else
            game = domain + "/?cmd=partida&sub=" + getURLParameter("sub") + "&download=partida";
    }
    else game = getURLParameter("game");

    if (game == null || game == "") {
        // Ask the user for a game input.
        var inputdiv = document.getElementById('inputdiv')
        inputdiv.style.display = "";
        document.getElementById('file').addEventListener('change', function(evt) {
            //http://www.html5rocks.com/en/tutorials/file/dndfiles/
            var file = evt.target.files[0];
            var reader = new FileReader();
            reader.readAsText(file);
            reader.onloadend = function(evt) {
                if (evt.target.readyState != FileReader.DONE) alert("Error accessing file.");
                else { // DONE == 2.
                    inputdiv.style.display = "none";
                    document.getElementById("loadingdiv").style.display = "";
                    initGame(reader.result);
                }
            };
        }, false);
    }
    else {
        document.getElementById("loadingdiv").style.display = "";
        // Load the given game.
        loadFile(game, initGame);
    }
}

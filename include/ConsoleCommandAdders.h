// this file should be included in the Main.cpp

engine.getConsole()->addCommand(ConsoleCommand("quit", "Shut down the program", "quit", quit));
engine.getConsole()->addCommand(ConsoleCommand("help", "List all available commands and their descriptions", "help", help));
engine.getConsole()->addCommand(ConsoleCommand("fps", "Show or hide the fps counter", "fps [0 | 1]", fps));
engine.getConsole()->addCommand(ConsoleCommand("wireframe", "Show scene as wireframe", "wireframe [0 | 1]", wireframe));
engine.getConsole()->addCommand(ConsoleCommand("skybox", "Show or hide skybox", "skybox [0 | 1]", skybox));
engine.getConsole()->addCommand(ConsoleCommand("arguments", "Print the arguments", "arguments [...]", arguments));
engine.getConsole()->addCommand(ConsoleCommand("consolePrefix", "Change the console input line prefix", "consolePrefix [string value]", consolePrefix));
engine.getConsole()->addCommand(ConsoleCommand("consoleSpeed", "Change the console's appearing/disappearing speed", "consoleSpeed [double value]", consoleSpeed));
engine.getConsole()->addCommand(ConsoleCommand("cameraFOV", "Set the camera field of view", "cameraFOV [int value]", cameraFOV));
engine.getConsole()->addCommand(ConsoleCommand("cameraPanel", "Show or hide the camera info", "cameraPanel [1 | 0]", cameraPanel));
engine.getConsole()->addCommand(ConsoleCommand("cameraAspect", "Set camera aspect ratio", "cameraAspect [double value | orig]", cameraAspect));
engine.getConsole()->addCommand(ConsoleCommand("cameraFly", "Release the camera for free flight", "cameraFly [0 | 1]", cameraFly));
engine.getConsole()->addCommand(ConsoleCommand("cameraPosition", "Set the camera's position (and orientation)", "cameraPosition [top | side | ground] | [double x] [double y] [double z]", cameraPosition));

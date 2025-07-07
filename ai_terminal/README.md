# AINUX Voice Terminal

This project integrates experimental AI features into the Linux kernel and user tools.
Recent commits introduced the `ai_core` kernel module and `modelfs` for managing on-device models,
as well as an Ollama client for local inference. The `ai_terminal` script now supports
voice commands and a context screen showing recent terminal history.

## Features
- Voice or text command input
- Execution of arbitrary shell commands
- Package installation with commands like `pip install openaicli`
- Opening a web browser
- Sending simple emails through `sendmail`
- Querying OpenAI about recent errors
- Viewing terminal history with the `show context` command

Set the `OPENAI_API_KEY` environment variable to enable OpenAI integration.

## Usage
```bash
python ai_terminal/main.py
```

## Future goal
Build the next AI interface for personal use on top of these components.

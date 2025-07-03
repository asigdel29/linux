# AI Voice Terminal

This is a simple demonstration of an AI-enabled terminal that responds to voice commands.
It uses the `speech_recognition` library to capture voice input and the OpenAI API for
context-aware answers.

Features include:

- Voice or text input
- Running shell commands
- Installing packages, e.g. `pip install openaicli`
- Opening a web browser
- Sending basic email via the local `sendmail`
- Asking questions about recent terminal errors using OpenAI

Set `OPENAI_API_KEY` in your environment for OpenAI integration.

```bash
python ai_terminal/main.py
```

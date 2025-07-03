import os
import subprocess
import webbrowser
import logging
from typing import Optional
from email.message import EmailMessage

try:
    import speech_recognition as sr
except ImportError:
    sr = None  # speech recognition is optional

try:
    import openai
except ImportError:
    openai = None  # openai sdk is optional

logging.basicConfig(level=logging.INFO)

TERMINAL_HISTORY = []


def listen() -> Optional[str]:
    """Capture voice input using the default microphone."""
    if sr is None:
        logging.warning("speech_recognition library not available")
        return input("command> ")

    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        logging.info("Listening...")
        audio = recognizer.listen(source)
    try:
        text = recognizer.recognize_google(audio)
        logging.info("Heard: %s", text)
        return text
    except sr.UnknownValueError:
        logging.error("Could not understand audio")
    except sr.RequestError as exc:
        logging.error("Speech recognition error: %s", exc)
    return None


def run_command(cmd: str):
    """Run a shell command and store the output."""
    logging.info("Running command: %s", cmd)
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    TERMINAL_HISTORY.append(f"$ {cmd}\n{result.stdout}\n{result.stderr}")
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(result.stderr)


def send_email(to_addr: str, subject: str, body: str):
    """Send an email using the local sendmail command."""
    msg = EmailMessage()
    msg['To'] = to_addr
    msg['From'] = os.getenv('USER', 'bot')
    msg['Subject'] = subject
    msg.set_content(body)

    process = subprocess.Popen(['/usr/sbin/sendmail', '-t', '-oi'], stdin=subprocess.PIPE)
    process.communicate(msg.as_bytes())


def ask_openai(question: str) -> str:
    """Query OpenAI with the terminal history as context."""
    if openai is None:
        logging.warning("openai library not available")
        return "OpenAI SDK not installed."

    api_key = os.getenv('OPENAI_API_KEY')
    if not api_key:
        return "OPENAI_API_KEY not set"
    openai.api_key = api_key

    prompt = f"You are a helpful assistant with terminal access. History:\n{''.join(TERMINAL_HISTORY)}\nUser question: {question}"
    response = openai.Completion.create(engine='text-davinci-003', prompt=prompt, max_tokens=150)
    return response.choices[0].text.strip()


def main():
    while True:
        command = listen()
        if not command:
            continue
        command = command.lower()
        if command in {'quit', 'exit'}:
            break
        elif 'install' in command and 'openaicli' in command:
            run_command('pip install openaicli')
        elif 'open browser' in command:
            webbrowser.open('https://www.google.com')
        elif command.startswith('send email'):
            parts = command.split()
            if len(parts) >= 4:
                to_addr = parts[2]
                subject = ' '.join(parts[3:])
                send_email(to_addr, subject, 'Sent from AI terminal')
        elif 'what is this error' in command:
            answer = ask_openai(command)
            print(answer)
        else:
            run_command(command)

if __name__ == '__main__':
    main()

import requests
import json
import os

class ChronoAgent:
	def __init__(self, character_name, system_profile):
		self.character_name = character_name
		self.system_profile = system_profile
		
		# Безопасное извлечение ключа API и URL модели исключительно из переменных окружения
		# В коде отсутствуют захардкоженные секреты.
		self.api_key = os.getenv("YANDEX_API_KEY", "PLACEHOLDER_KEY")
		self.model_uri = os.getenv("YANDEX_MODEL_URI", "PLACEHOLDER_URI")
		self.url = "https://llm.api.cloud.yandex.net/foundationModels/v1/completion"

	def generate_reply(self, user_input, temperature=0.45):
		if self.api_key == "PLACEHOLDER_KEY" or self.model_uri == "PLACEHOLDER_URI":
			return "Error: YANDEX_API_KEY or YANDEX_MODEL_URI environment variables are not set."

		headers = {
			"Content-Type": "application/json",
			"Authorization": f"Api-Key {self.api_key}"
		}

		payload = {
			"modelUri": self.model_uri,
			"completionOptions": {
				"stream": False,
				"temperature": temperature,
				"maxTokens": "500"
			},
			"messages": [
				{
					"role": "system",
					"text": f"Ты — {self.character_name}. {self.system_profile} Ограничься строго прямой речью и мыслями в круглых скобках (...), без описания твоих действий в звездочках или стороннего текста."
				},
				{
					"role": "user",
					"text": user_input
				}
			]
		}
		
		try:
			response = requests.post(self.url, headers=headers, json=payload)
			if response.status_code == 200:
				result = response.json()
				return result["result"]["alternatives"][0]["message"]["text"]
			else:
				return f"Error: Code {response.status_code}, Message: {response.text}"
		except Exception as e:
			return f"Exception occurred: {str(e)}"

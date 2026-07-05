import os
import requests

class ChronoAgent:
	def __init__(self, character_name, public_profile, private_secrets):
		self.character_name = character_name
		self.public_profile = public_profile
		self.private_secrets = private_secrets # Секретные знания, скрытые от других агентов
		
		# Чтение системных переменных для защиты ключей
		self.api_key = os.getenv("YANDEX_API_KEY", "PLACEHOLDER_KEY")
		self.model_uri = os.getenv("YANDEX_MODEL_URI", "PLACEHOLDER_URI")
		self.url = "https://llm.api.cloud.yandex.net/foundationModels/v1/completion"

	def generate_hidden_thought(self, public_history):
		"""
		ФАЗА 1: Генерация скрытой внутренней мысли (Chain of Thought).
		Этот результат записывается ТОЛЬКО в приватный лог в оперативной памяти игры.
		Важно: Промпт четко регламентирует, что агент знает только о СЕБЕ и СВОИХ секретах,
		и пишет мысли исключительно от первого лица.
		"""
		prompt = (
			f"Ты — {self.character_name}. {self.public_profile}\n"
			f"Твои глубокие скрытые секреты, о которых знаешь ТОЛЬКО ТЫ и никто другой: {self.private_secrets}\n"
			f"Ты не знаешь чужих секретов. Ты реагируешь исключительно от своего лица.\n"
			f"Напиши свои текущие внутренние мысли от первого лица, выражая то, что ты чувствуешь "
			f"в ответ на последнюю ситуацию. Ограничься строго мыслями без прямой речи и мета-текста."
		)
		return self._request_llm(prompt, public_history, temperature=0.6)

	def generate_public_dialogue(self, public_history, personal_thought):
		"""
		ФАЗА 2: Генерация реплики вслух на основе скрытой мысли.
		Включает отображение собственных мыслей в формате [i](мысли)[/i], но полностью скрывает чужие секреты.
		"""
		prompt = (
			f"Ты — {self.character_name}. {self.public_profile}\n"
			f"Твоя текущая внутренняя мысль: {personal_thought}\n"
			f"Сформулируй реплику для визуальной новеллы. Сначала напиши свои мысли строго внутри тегов [i](мысли)[/i], "
			f"а затем напиши фразу, которую ты произносишь вслух. Ограничься строго этим текстом, без мета-описаний."
		)
		return self._request_llm(prompt, public_history, temperature=0.4)

	def _request_llm(self, system_text, user_text, temperature):
		if self.api_key == "PLACEHOLDER_KEY" or self.model_uri == "PLACEHOLDER_URI":
			return "[Error: API key or Model URI not set]"

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
				{"role": "system", "text": system_text},
				{"role": "user", "text": str(user_text)}
			]
		}

		try:
			response = requests.post(self.url, headers=headers, json=payload)
			if response.status_code == 200:
				return response.json()["result"]["alternatives"][0]["message"]["text"].strip()
			return f"[Error Code {response.status_code}]"
		except Exception as e:
			return f"[Exception: {str(e)}]"

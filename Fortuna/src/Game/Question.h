#pragma once

#include <nlohmann/json.hpp>

struct Question
{
	std::string category;
	std::string text;
};

inline void from_json(const nlohmann::json& j, Question& q)
{
	j.at("category").get_to(q.category);
	j.at("text").get_to(q.text);
}

inline void to_json(nlohmann::json& j, const Question& q)
{
	j = nlohmann::json{
		{"category", q.category},
		{"text", q.text}
	};
}

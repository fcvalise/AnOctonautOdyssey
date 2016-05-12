#ifndef TEXTMANAGER_HPP
# define TEXTMANAGER_HPP

# include <vector>
# include <map>
# include "ResourceDefinitions.hpp"

class TextManager
{
public:
	TextManager(void);
	~TextManager(void) = default;

	static TextManager & getInstance(void);
	void loadTexts(void);
	std::vector<std::wstring> const & getTexts(ResourceKey const & key);
	std::vector<std::wstring> const & getTexts(std::string const & key);

private:
	static std::unique_ptr<TextManager>					m_instance;
	std::map<std::string, std::vector<std::wstring>>	m_texts;
};

#endif

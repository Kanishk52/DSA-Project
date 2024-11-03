const searchBox = document.getElementById("searchBox");
const suggestionsContainer = document.getElementById("suggestions");

searchBox.addEventListener("input", async () => {
    const query = searchBox.value.trim();
    
    // Extract the last word for autocomplete suggestions
    const lastWord = query.split(" ").pop();
    if (lastWord.length === 0) {
        suggestionsContainer.style.display = "none";
        return;
    }

    try {
        // Send a request to the server for suggestions based on the last word
        const response = await fetch(`/suggestions?query=${encodeURIComponent(lastWord)}`);
        if (response.ok) {
            const suggestions = await response.json();

            // Clear previous suggestions
            suggestionsContainer.innerHTML = "";
            if (suggestions.length > 0) {
                suggestionsContainer.style.display = "block";
                suggestions.forEach(suggestion => {
                    const suggestionItem = document.createElement("div");
                    suggestionItem.classList.add("suggestion-item");
                    
                    // Replace only the last word with the suggestion
                    const fullQuery = query.replace(/(\S+)$/, suggestion);
                    suggestionItem.textContent = suggestion;
                    suggestionItem.onclick = () => {
                        searchBox.value = fullQuery + " ";
                        suggestionsContainer.style.display = "none";
                    };
                    suggestionsContainer.appendChild(suggestionItem);
                });
            } else {
                suggestionsContainer.style.display = "none";
            }
        } else {
            console.error("Failed to fetch suggestions");
        }
    } catch (error) {
        console.error("Error fetching suggestions:", error);
    }
});

// Hide suggestions when clicking outside
document.addEventListener("click", (event) => {
    if (event.target !== searchBox) {
        suggestionsContainer.style.display = "none";
    }
});

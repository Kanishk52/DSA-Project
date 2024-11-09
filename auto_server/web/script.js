// Get DOM elements
const searchBox = document.getElementById("searchBox");
const suggestionsContainer = document.getElementById("suggestions");
const newWordInput = document.getElementById("newWord");
const addWordBtn = document.getElementById("addWordBtn");
const addStatus = document.getElementById("addStatus");

let currentSuggestions = [];
let selectedSuggestionIndex = -1;

// Add word function
async function addNewWord() {
    const word = newWordInput.value.trim().toLowerCase();
    
    if (!word) {
        addStatus.textContent = "Please enter a word";
        addStatus.style.color = "red";
        return;
    }

    const response = await fetch(`/add-word?word=${encodeURIComponent(word)}`, {
        method: 'POST'
    });

    if (response.ok) {
        const result = await response.json();
        
        if (result.status === "word_added") {
            addStatus.textContent = "Word added successfully!";
            addStatus.style.color = "green";
            newWordInput.value = "";
            setTimeout(() => { addStatus.textContent = ""; }, 3000);
        } else {
            addStatus.textContent = `Failed to add word: ${result.message || "Unknown reason"}`;
            addStatus.style.color = "red";
        }
    } else {
        addStatus.textContent = `Error: ${response.status} ${response.statusText}`;
        addStatus.style.color = "red";
    }
}

// Add click event listener to the Add Word button
addWordBtn.addEventListener("click", addNewWord);

// Add keyboard event listener for the new word input
newWordInput.addEventListener("keypress", (event) => {
    if (event.key === "Enter") {
        event.preventDefault();
        addNewWord();
    }
});

// Autocomplete functionality
searchBox.addEventListener("input", async () => {
    const query = searchBox.value.trim();
    const lastWord = query.split(" ").pop();
    selectedSuggestionIndex = -1;
    
    if (lastWord.length === 0) {
        suggestionsContainer.style.display = "none";
        currentSuggestions = [];
        return;
    }
    
    try {
        const response = await fetch(`/suggestions?query=${encodeURIComponent(lastWord)}`);
        
        if (response.ok) {
            currentSuggestions = await response.json();
            suggestionsContainer.innerHTML = "";
            
            if (currentSuggestions.length > 0) {
                suggestionsContainer.style.display = "block";
                
                currentSuggestions.forEach((suggestion, index) => {
                    const suggestionItem = document.createElement("div");
                    suggestionItem.classList.add("suggestion-item");
                    
                    const fullQuery = query.replace(/(\S+)$/, suggestion);
                    suggestionItem.textContent = fullQuery;
                    
                    suggestionItem.onclick = () => {
                        searchBox.value = fullQuery + " ";
                        suggestionsContainer.style.display = "none";
                        currentSuggestions = [];
                    };
                    
                    suggestionsContainer.appendChild(suggestionItem);
                });
            } else {
                suggestionsContainer.style.display = "none";
                currentSuggestions = [];
            }
        }
    } catch (error) {
        console.error("Error fetching suggestions:", error);
    }
});

// Keyboard navigation
searchBox.addEventListener("keydown", (event) => {
    if (event.key === "Tab") {
        event.preventDefault();
        if (currentSuggestions.length > 0) {
            const query = searchBox.value.trim();
            const selectedSuggestion = currentSuggestions[selectedSuggestionIndex !== -1 ? 
                selectedSuggestionIndex : 0];
            const fullQuery = query.replace(/(\S+)$/, selectedSuggestion);
            searchBox.value = fullQuery + " ";
            suggestionsContainer.style.display = "none";
            currentSuggestions = [];
            selectedSuggestionIndex = -1;
        }
    }
    else if (event.key === "ArrowDown" || event.key === "ArrowUp") {
        event.preventDefault();
        if (currentSuggestions.length > 0) {
            if (event.key === "ArrowDown") {
                selectedSuggestionIndex = (selectedSuggestionIndex + 1) % currentSuggestions.length;
            } else {
                selectedSuggestionIndex = selectedSuggestionIndex <= 0 ? 
                    currentSuggestions.length - 1 : selectedSuggestionIndex - 1;
            }
            
            const suggestionItems = suggestionsContainer.getElementsByClassName("suggestion-item");
            Array.from(suggestionItems).forEach((item, index) => {
                if (index === selectedSuggestionIndex) {
                    item.classList.add("selected");
                    searchBox.value = item.textContent;
                } else {
                    item.classList.remove("selected");
                }
            });
        }
    }
});

// Click outside handler
document.addEventListener("click", (event) => {
    if (event.target !== searchBox) {
        suggestionsContainer.style.display = "none";
        currentSuggestions = [];
        selectedSuggestionIndex = -1;
    }
});
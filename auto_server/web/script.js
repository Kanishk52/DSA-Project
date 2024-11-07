// Get DOM elements
const searchBox = document.getElementById("searchBox");
const suggestionsContainer = document.getElementById("suggestions");

// Add new elements for word addition
const addWordForm = document.createElement("div");
addWordForm.innerHTML = `
    <input type="text" id="newWord" placeholder="Enter a new word" class="word-input">
    <button id="addWordBtn" class="add-btn">Add Word</button>
    <div id="addStatus" class="status-message"></div>
`;
document.body.insertBefore(addWordForm, suggestionsContainer.nextSibling);

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
    
    const result = await response.json();
    
    if (result.status === "word_added") {
        addStatus.textContent = "Word added successfully!";
        addStatus.style.color = "green";
        newWordInput.value = "";
        // Clear status message after 3 seconds
        setTimeout(() => {
            addStatus.textContent = "";
        }, 3000);
    } else {
        addStatus.textContent = "Failed to add word";
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

// Existing autocomplete functionality
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
                    suggestionItem.textContent = suggestion;
                    
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

// Existing keyboard navigation
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
                } else {
                    item.classList.remove("selected");
                }
            });
        }
    }
});

// Existing click outside handler
document.addEventListener("click", (event) => {
    if (event.target !== searchBox) {
        suggestionsContainer.style.display = "none";
        currentSuggestions = [];
        selectedSuggestionIndex = -1;
    }
});
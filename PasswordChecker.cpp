String^ PasswordChecker::GetLeetSpeakFeedback(String^ password)
{
    if (!IsLeetSpeakVariant(password))
        return "";

    String^ normalized = NormalizeLeetSpeak(password);
    
    return "⚠ Leet Speak aniqlandi: \"" + password + 
           "\" → \"" + normalized + "\"\n" +
           "Bu usul zamonaviy hujum vositalariga to'siq bo'la olmaydi.\n" +
           "Tavsiya: Butunlay tasodifiy va lug'atda bo'lmagan paroldan foydalaning.";
}
int PasswordChecker::ApplyLeetSpeakPenalties(
    String^ password, PasswordResult^ result)
{
    int penalty = 0;

    if (!IsLeetSpeakVariant(password))
        return 0;

    result->IsLeetSpeak = true;
    String^ normalized  = NormalizeLeetSpeak(password);
    int     replacements = CountLeetReplacements(password);
    double  density = (double)replacements / password->Length;

    // Holat 1: Zaif so'zning to'liq Leet Speak varianti
    array<String^>^ exactWeakWords = {
        "password", "admin", "parol", "tashkent",
        "uzbekiston", "iloveyou", "qwerty", "letmein"
    };
    for each (String^ word in exactWeakWords)
    {
        if (normalized == word)
        {
            penalty = 40;
            result->Feedback->Add(
                "⚠ Leet Speak aniqlandi: \"" + password +
                "\" → \"" + normalized + "\"\n"
                "  Zamonaviy vositalar bu almashtirishlarni "
                "avtomatik sinab ko'radi.");
            return penalty;
        }
    }

    // Holat 2: Leet Speak + sana naqshi kombinatsiyasi
    if (result->HasPattern && density >= 0.2)
    {
        penalty = 45;
        result->Feedback->Add(
            "⚠ Leet Speak va naqsh kombinatsiyasi aniqlandi. "
            "Bu parol ayniqsa zaif hisoblanadi.");
        return penalty;
    }

    // Holat 3: Qisman moslik (zaif so'z parol ichida)
    if (density >= 0.3)
    {
        penalty = 25;
        result->Feedback->Add(
            "⚠ Parolda Leet Speak almashtirishlari aniqlandi. "
            "Bu usul yetarli himoya ta'minlamaydi.");
    }
    else if (replacements <= 2)
    {
        penalty = 10;
        result->Feedback->Add(
            "ℹ Minimal Leet Speak almashtirishlari topildi.");
    }

    return penalty;
}

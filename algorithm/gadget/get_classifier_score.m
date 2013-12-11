function get_classifier_score (pred_probs, true_labels)

for t = 0: 0.1: 1
    lt = true_labels;
    lt(pred_probs >= t) = 1;
    lt(pred_probs < t) = -1;
    tp = length(find(lt == 1 & true_labels == 1));
    fp = length(find(lt == 1 & true_labels == -1));
    fn = length(find(lt == -1 & true_labels == 1));
    prec = tp / (tp + fp);
    rec = tp / (tp + fn);
    f = 2 * prec * rec / (prec + rec);
    fprintf('t = %f, p = %f, r = %f, f = %f\n', t, prec, rec, f);
end